/*

Dimitre Lima 2019
Uncompressed tiff writer for openFrameworks
Only for RGB888 big Endian

*/

class tiffFastWriter {
public:
	// not working with bigEndian = false yet.
	bool bigEndian = true;
	
	uint32_t flip(uint32_t x) {
//		return bigEndian ? EndianU32_LtoB(x) : x;
		return bigEndian ? __builtin_bswap32(x) : x;
		
	}
	
	uint16_t flip(uint16_t x) {
//		return bigEndian ? EndianU16_LtoB(x) : x;
		return bigEndian ? __builtin_bswap16(x) : x;
	}
	
	string tiffEnd(string input) {
		string saida;
		if (bigEndian) {
			saida = input;
		} else {
			saida = "";
			saida += input.substr(2,2);
			saida += input.substr(0,2);
			saida += input.substr(6,2);
			saida += input.substr(4,2);
			saida += input.substr(10,2);
			saida += input.substr(8,2);
		}
		return ofHexToString(saida);
	}
	
	string tiffTag(string input) {
		string saida;
		if (bigEndian) {
			saida = input;
		} else {
			saida = "";
			saida += input.substr(2,2);
			saida += input.substr(0,2);
			saida += input.substr(6,2);
			saida += input.substr(4,2);
			saida += input.substr(14,2);
			saida += input.substr(12,2);
			saida += input.substr(10,2);
			saida += input.substr(8,2);
		}
		return ofHexToString(saida);
	}
	
	string tiffParam(string input) {
		string saida;
		if (bigEndian) {
			saida = input;
		} else {
			saida = "";
			saida += input.substr(2,2);
			saida += input.substr(0,2);
		}
		return ofHexToString(saida);
	}
	
	string tiffParamLarge(string input) {
		string saida;
		if (bigEndian) {
			saida = input;
		} else {
			saida = "";
			saida += input.substr(6,2);
			saida += input.substr(4,2);
			saida += input.substr(2,2);
			saida += input.substr(0,2);
		}
		return ofHexToString(saida);
	}
	
	
	void recordTiff(ofPixels * pixels, string fullFileName) {
		uint16_t width = pixels->getWidth();
		uint16_t height = pixels->getHeight();
		size_t size = pixels->getWidth() * pixels->getHeight() * 3;
		
		ofFile f;
		f.open(fullFileName, ofFile::WriteOnly);
		ofBuffer buffer;
		
		if (bigEndian) {
			buffer.append(ofHexToString("4d4d002a")); // big endian MM
		} else {
			buffer.append(ofHexToString("49492a00")); // little endian II
		}
		
		{
			uint32_t offset = size + 8;
			offset = flip(offset);
			buffer.append((const char *)&offset, 4);
		}
		
		//  ALL BITMAP RGB
		buffer.append((char*)pixels->getData(),size);
		
		// FOOTER
		// number of entries (tags) = 14
		// buffer.append(tiffParam("000e"));
		uint16_t numberoftags = 14;
		numberoftags = flip(numberoftags);
		buffer.append((const char *)&numberoftags, 2);
		
		/* Width tag, short int */
		buffer.append(tiffTag("0100000300000001")); //8 bytes
		uint16_t w = width;
		w = flip(w);
		buffer.append((const char *)&w, 2);
		buffer.append(ofHexToString("0000")); //close tag
		
		// img height tag
		buffer.append(tiffTag("0101000300000001"));
		uint16_t h = height;
		h = flip(h);
		buffer.append((const char *)&h, 2);
		buffer.append(ofHexToString("0000"));
		
		/* Bits per sample tag, short int */
		buffer.append(tiffTag("0102000300000003"));
		{
			uint32_t offset = size + 182;
			offset = flip(offset);
			buffer.append((const char *)&offset, 4);
		}
		
		/* Compression flag, short int */
		buffer.append(ofHexToString("010300030000000100010000")); //close tag
		
		/* Photometric interpolation tag, short int */
		buffer.append(ofHexToString("010600030000000100020000")); //close tag
		
		/* Strip offset tag, long int */
		buffer.append(ofHexToString("011100040000000100000008"));
		
		/* Orientation flag, short int */
		buffer.append(ofHexToString("011200030000000100010000"));
		
		/* Sample per pixel tag, short int */
		buffer.append(ofHexToString("011500030000000100030000"));
		
		/* Rows per strip tag, short int */
		buffer.append(tiffTag("0116000300000001"));
		{
			uint16_t w = width;
			w = flip(w);
			buffer.append((const char *)&w, 2);
		}
		buffer.append(ofHexToString("0000"));
		
		/* Strip byte count flag, long int */
		buffer.append(tiffTag("0117000400000001"));
		{
			uint32_t offset = size;
			offset = flip(offset);
			buffer.append((const char *)&offset, 4);
		}
		//buffer.append(ofHexToString("0000"));
		
		/* Minimum sample value flag, short int */
		buffer.append(tiffTag("0118000300000003"));
		{
			uint32_t offset = size + 188;
			offset = flip(offset);
			buffer.append((const char *)&offset, 4);
		}
		//buffer.append(ofHexToString("0000"));
		
		/* Maximum sample value tag, short int */
		buffer.append(tiffTag("0119000300000003"));
		{
			uint32_t offset = size + 194;
			offset = flip(offset);
			buffer.append((const char *)&offset, 4);
		}
		//buffer.append(ofHexToString("0000"));
		
		/* Planar configuration tag, short int */
		buffer.append(ofHexToString("011c00030000000100010000"));
		
		/* Sample format tag, short int */
		buffer.append(tiffTag("0153000300000003"));
		{
			uint32_t offset = size + 200;
			offset = flip(offset);
			buffer.append((const char *)&offset, 4);
		}
		
		/* End of the directory entry */
		buffer.append(ofHexToString("00000000"));
		
		/* Bits for each colour channel */
		buffer.append(tiffEnd("000800080008"));
		
		/* Minimum value for each component */
		buffer.append(tiffEnd("000000000000"));
		
		/* Maximum value per channel */
		buffer.append(tiffEnd("00ff00ff00ff"));
		
		/* Samples per pixel for each channel */
		buffer.append(tiffEnd("000100010001"));
		
		f.writeFromBuffer(buffer);
		f.close();
	}
};
