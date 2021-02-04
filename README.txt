Finds TVPXP3ArchiveExtractionFilter on any krkr2, krkrz games. It is used to encrypt and decrypt xp3 archive data.

void TVPXP3ArchiveExtractionFilter(tTVPXP3ExtractionFilterInfo info);

#pragma pack(push, 4)
struct tTVPXP3ExtractionFilterInfo
{
	const tjs_uint SizeOfSelf; // structure size of tTVPXP3ExtractionFilterInfo itself
	const tjs_uint64 Offset; // offset of the buffer data in uncompressed stream position
	void * Buffer; // target data buffer
	const tjs_uint BufferSize; // buffer size in bytes pointed by "Buffer"
	const tjs_uint32 FileHash; // hash value of the file (since inteface v2)

	tTVPXP3ExtractionFilterInfo(tjs_uint64 offset, void *buffer,
		tjs_uint buffersize, tjs_uint32 filehash) :
			Offset(offset), Buffer(buffer), BufferSize(buffersize),
			FileHash(filehash),
			SizeOfSelf(sizeof(tTVPXP3ExtractionFilterInfo)) {;}
};
#pragma pack(pop)