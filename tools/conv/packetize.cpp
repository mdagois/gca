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

enum : uint8_t
{
	kMagic0 = 0x88,
	kMagic1 = 0x33,
	kCommandType = 0x04,
	kCompressionFlag = 0x00,
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
	kPacketField_ChecksumLSB = kPacketField_PayloadStart + kDataPacketPayloadSize,
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
	uint8_t* file_data = nullptr;
	uint32_t file_data_size = 0;
	if(!LoadFile(file_data, file_data_size, filename))
	{
		cout << "Could not load file [" << filename << "]" << endl;
		return 1;
	}

	if(file_data_size % kTileSize != 0)
	{
		cout << "The data size must be a multiple of a single tile size [" << kTileSize << "] in [" << filename << "]" << endl;
		return 1;
	}

	if(file_data_size % kDataPacketPayloadSize != 0)
	{
		cout << "The data size must be a multiple of the maximum packet payload [" << kDataPacketPayloadSize << "] in [" << filename << "]" << endl;
		return 1;
	}

	const uint32_t packet_count = file_data_size / kDataPacketPayloadSize;
	const uint32_t output_size = kPacketSize * packet_count;
	uint8_t* output = new uint8_t[output_size];
	for(uint32_t i = 0; i < packet_count; ++i)
	{
		uint8_t* const packet = output + i * kPacketSize;

		packet[kPacketField_Magic0] = kMagic0;
		packet[kPacketField_Magic1] = kMagic1;
		packet[kPacketField_CommandType] = kCommandType;
		packet[kPacketField_CompressionFlag] = kCompressionFlag;
		packet[kPacketField_PayloadSizeLSB] = kDataPacketPayloadSize & 0xFF;
		packet[kPacketField_PayloadSizeMSB] = (kDataPacketPayloadSize >> 8) & 0xFF;

		memcpy(packet + kPacketField_PayloadStart, file_data + i * kDataPacketPayloadSize, kDataPacketPayloadSize);

		uint32_t checksum = 0;
		checksum += packet[kPacketField_CommandType];
		checksum += packet[kPacketField_CompressionFlag];
		checksum += packet[kPacketField_PayloadSizeLSB];
		checksum += packet[kPacketField_PayloadSizeMSB];
		for(uint32_t i = 0; i < kDataPacketPayloadSize; ++i)
		{
			checksum += packet[kPacketField_PayloadStart + i];
		}

		packet[kPacketField_ChecksumLSB] = checksum & 0xFF;
		packet[kPacketField_ChecksumMSB] = (checksum >> 8) & 0xFF;

		packet[kPacketField_Ack] = 0x00;
		packet[kPacketField_Status] = 0x00;
	}

	const bool success = writeOutputData(output, output_size, getOutputFilename(filename, ".pkt").c_str());
	return success ? 0 : 1;
}

