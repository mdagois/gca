#include <cassert>
#include <iostream>
#include <string>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

enum : uint32_t
{
	kTileSize = 16,
	kDataPacketPayloadSize = 640,
	kPacketByteOverhead = 10,
	kPacketSize = kDataPacketPayloadSize + kPacketByteOverhead,
};

enum PacketField : uint32_t
{
	kPacketField_Magic0,
	kPacketField_Magic1,
	kPacketField_CommandType,
	kPacketField_CompressionFlag,
	kPacketField_PayloadSizeLSB,
	kPacketField_PayloadSizeMSB,
	kPacketField_PayloadStart,
	kPacketField_PayloadEnd = kPacketField_PayloadStart + kDataPacketPayloadSize,
	kPacketField_ChecksumLSB,
	kPacketField_ChecksumMSB,
	kPacketField_Ack,
	kPacketField_Status,
};

////////////////////////////////////////////////////////////////////////////////
// File
////////////////////////////////////////////////////////////////////////////////

enum : uint32_t
{
	kExtensionLength = 4,
};

static string getOutputFilename(const char* filename, const char* extension)
{
	assert(strlen(extension) == kExtensionLength);

	string output = filename;
	if(output.find_last_of('.') == output.size() - kExtensionLength)
	{
		output.pop_back();
		output.pop_back();
		output.pop_back();
		output.pop_back();
	}
	output.append(extension);
	return output;
}

static bool LoadFile(uint8_t*& out_data, uint32_t& out_data_size, const char* filename)
{
	assert(filename != nullptr);

	FILE* file = fopen(filename, "rb");
	if(file == nullptr)
	{
		cout << "Could not open file [" << filename << "] for read" << endl;
		return false;
	}

	fseek(file, 0, SEEK_END);
	const long int length = ftell(file);
	if(length <= 0)
	{
		cout << "File size if zero [" << filename << "]" << endl;
		fclose(file);
		return false;
	}
	fseek(file, 0, SEEK_SET);

	uint8_t* data = new uint8_t[length];
	if(data == nullptr)
	{
		cout << "Could not allocate enough memory [" << length << "] for [" << filename << "]" << endl;
		fclose(file);
		return false;
	}

	size_t n = fread(data, length, 1, file);
	fclose(file);
	if(n != 1)
	{
		cout << "Could not read the data for file [" << filename << "]" << endl;
		delete [] data;
		return false;
	}

	out_data = data;
	out_data_size = length;
	return true;
}

static bool writeOutputData(const uint8_t* data, uint32_t data_size, const char* filename)
{
	FILE* file = fopen(filename, "wb");
	if(!file)
	{
		cout << "Could not open file [" << filename << "]" << endl;
		return false;
	}

	const size_t written = fwrite(data, 1, data_size, file);
	fclose(file);
	return written == data_size;
}

////////////////////////////////////////////////////////////////////////////////
// Checksum
////////////////////////////////////////////////////////////////////////////////

static uint16_t ComputeChecksum(const uint8_t* data, uint32_t data_size)
{
	uint16_t checksum = 0;
	for(uint32_t i = 0; i < data_size; ++i)
	{
		checksum += data[i];
	}
	return checksum;
}

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		cout
			<< "USAGE" << endl
			<< argv[0] << " <tile_binary_filename>" << endl;
		return 0;
	}

	const char* filename = argv[1];
	uint8_t* data = nullptr;
	uint32_t data_size = 0;
	if(!LoadFile(data, data_size, filename))
	{
		cout << "Could not load file [" << filename << "]" << endl;
		return 1;
	}

	if(data_size % kTileSize != 0)
	{
		cout << "The data size must be a multiple of a single tile size [" << kTileSize << "] in [" << filename << "]" << endl;
		return 1;
	}

	if(data_size % kDataPacketPayloadSize != 0)
	{
		cout << "The data size must be a multiple of the maximum packet payload [" << kDataPacketPayloadSize << "] in [" << filename << "]" << endl;
		return 1;
	}

	const uint32_t packet_count = data_size / kDataPacketPayloadSize;
	const uint32_t output_size = kPacketSize * packet_count;
	uint8_t* output = new uint8_t[output_size];
	for(uint32_t i = 0; i < packet_count; ++i)
	{
		output[kPacketField_Magic0] = 0x88;
		output[kPacketField_Magic1] = 0x33;
		output[kPacketField_CommandType] = 0x04;
		output[kPacketField_CompressionFlag] = 0x00;
		output[kPacketField_PayloadSizeLSB] = kDataPacketPayloadSize & 0xFF;
		output[kPacketField_PayloadSizeMSB] = (kDataPacketPayloadSize >> 8) & 0xFF;

		memcpy(output + kPacketField_PayloadStart, data, kDataPacketPayloadSize);

		const uint16_t checksum =
			output[kPacketField_CommandType] +
			output[kPacketField_CompressionFlag] +
			output[kPacketField_PayloadSizeLSB] +
			output[kPacketField_PayloadSizeMSB] +
			ComputeChecksum(data, kDataPacketPayloadSize);
		output[kPacketField_ChecksumLSB] = checksum & 0xFF;
		output[kPacketField_ChecksumMSB] = (checksum >> 8) & 0xFF;

		output[kPacketField_Ack] = 0x00;
		output[kPacketField_Status] = 0x00;

		data += kDataPacketPayloadSize;
		output += kPacketSize;
	}

	const bool success = writeOutputData(output, output_size, getOutputFilename(filename, ".pkt").c_str());
	return success ? 0 : 1;
}

