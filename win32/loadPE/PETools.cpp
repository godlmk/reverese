#include "PETools.h"
#include <print>
#include <cassert>
#include <format>

char errStr[0x20];

int Align(int origin, int alignment)
{
	return ((origin + alignment - 1) / alignment) * alignment;
}

PIMAGE_DOS_HEADER GetDosHeader(LPVOID pImageBuffer)
{
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)pImageBuffer;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		std::println("Invalid DOS signature");
		return NULL;
	}
	return dosHeader;
}
PIMAGE_NT_HEADERS GetNTHeader(LPVOID pImageBuffer, PIMAGE_DOS_HEADER dosHeader)
{
	const PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)((DWORD)pImageBuffer + dosHeader->e_lfanew);
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
		std::println("Invalid NT signature");
		return NULL;
	}
	return ntHeader;
}
DWORD ReadFileBuffer(LPCTSTR filename, void** pBuffer) {
	FILE* fp;
	errno_t err = _wfopen_s(&fp, filename, L"rb");
	if (fp == NULL)
	{
		strerror_s(errStr, 0x20, errno);
		std::println("fread failed, because:{}", errStr);
		exit(-1);
	}
	fseek(fp, 0, SEEK_END);
	long bytes = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	unsigned char* buffer = (unsigned char*)malloc(bytes);
	if (!buffer) {
		strerror_s(errStr, 0x20, errno);
		std::println("malloc failed, because:{}", errStr);
		fclose(fp);
		exit(-1);
	}
	int ret = fread(buffer, bytes, 1, fp);
	if (ret != 1) {
		strerror_s(errStr, 0x20, errno);
		std::println("fread failed, because:{}", errStr);
		free(buffer);
		fclose(fp);
		exit(-1);
	}
	fclose(fp);
	*pBuffer = buffer;
	return bytes;
}

bool ImageMemory2File(PBYTE pMemBuffer, const wchar_t* destPath) {
	const PIMAGE_DOS_HEADER const dosHeader = (PIMAGE_DOS_HEADER)pMemBuffer;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		std::println("has not dos format");
		return false;
	}
	const PIMAGE_NT_HEADERS const ntHeader = (PIMAGE_NT_HEADERS)((DWORD)pMemBuffer + dosHeader->e_lfanew);
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
		std::println("Invalid NT signature");
		return false;
	}
	const PIMAGE_OPTIONAL_HEADER const optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)ntHeader
		+ sizeof(ntHeader->Signature)
		+ sizeof(ntHeader->FileHeader));// ntHeader->OptionalHeader;
	assert(optionalHeader == &ntHeader->OptionalHeader, "optionalHeader != ntHeader->OptionalHeader");
	DWORD fileSize = 0;
	// ��������header�ͽڱ�Ĵ�С
	fileSize += optionalHeader->SizeOfHeaders;
	//	����ÿһ�ڵĴ�С
	const size_t sectionCount = ntHeader->FileHeader.NumberOfSections;
	const PIMAGE_SECTION_HEADER const firstSection = IMAGE_FIRST_SECTION(ntHeader);
	for (size_t i = 0; i < sectionCount; ++i) {
		const PIMAGE_SECTION_HEADER const curSection = firstSection + i;
		fileSize += curSection->SizeOfRawData;
	}
	PBYTE pFileBuffer = (PBYTE)malloc(fileSize);
	if (!pFileBuffer) {
		strerror_s(errStr, 0x20, errno);
		std::println("malloc failed, because:{}", errStr);
		return false;
	}
	memset(pFileBuffer, 0, fileSize);
	// �������е�ͷ�ͽڱ�
	memcpy(pFileBuffer, pMemBuffer, optionalHeader->SizeOfHeaders);
	for (size_t i = 0; i < sectionCount; ++i) {
		const PIMAGE_SECTION_HEADER const curSection = firstSection + i;
		const DWORD offsetInMemry = curSection->VirtualAddress;
		const DWORD offsetInFile = curSection->PointerToRawData;
		const DWORD curSectionSizeinFile = curSection->SizeOfRawData;
		PBYTE dest = pFileBuffer + offsetInFile;
		PBYTE src = pMemBuffer + offsetInMemry;
		memcpy(dest, src, curSectionSizeinFile);
	}
	FILE* fp;
	errno_t err = _wfopen_s(&fp, destPath, L"wb");
	if (!fp) {
		strerror_s(errStr, 0x20, errno);
		std::println("fopen failed, because:{}", errStr);
		return false;
	}
	size_t ret = fwrite(pFileBuffer, fileSize, 1, fp);
	if (ret != 1) {
		strerror_s(errStr, 0x20, errno);
		std::println("fwrite failed, because:{}", errStr);
		fclose(fp);
		return false;
	}
	fclose(fp);
	free(pFileBuffer);
	return true;
}

PBYTE ReadMemoryImage(LPCTSTR filename) {
	unsigned char* buffer;
	DWORD size = ReadFileBuffer(filename, (void**)&buffer);
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)buffer;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		free(buffer);
		std::println("Invalid DOS signature\n");
		return NULL;
	}
	const PIMAGE_NT_HEADERS const ntHeader = (PIMAGE_NT_HEADERS)(buffer + dosHeader->e_lfanew);
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
		free(buffer);
		std::println("Invalid NT signature\n");
		return NULL;
	}
	const PIMAGE_OPTIONAL_HEADER const optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)ntHeader
		+ sizeof(ntHeader->Signature)
		+ sizeof(ntHeader->FileHeader));// ntHeader->OptionalHeader;
	std::println("offset is {:X}, use & is {:X}", (DWORD)optionalHeader, (DWORD)&ntHeader->OptionalHeader);
	assert(optionalHeader == &ntHeader->OptionalHeader);
	const DWORD ImageSize = optionalHeader->SizeOfImage;
	PBYTE const ImageBuffer = (PBYTE)malloc(ImageSize);
	if (!ImageBuffer) {
		free(buffer);
		strerror_s(errStr, 0x20, errno);
		std::println("malloc failed, because:{}", errStr);
		return NULL;
	}
	memset(ImageBuffer, 0, ImageSize);
	// �������е�ͷ�ͽڱ�
	const DWORD sizeOfHeaderAndSection = optionalHeader->SizeOfHeaders;
	memcpy(ImageBuffer, buffer, sizeOfHeaderAndSection);
	// ����ÿһ�ڵ����ݵ�Ӧ���ڵ�λ��
	const PIMAGE_SECTION_HEADER const firstSection = IMAGE_FIRST_SECTION(ntHeader);
	const int sectionCount = ntHeader->FileHeader.NumberOfSections;
	for (int i = 0; i < sectionCount; ++i) {
		PIMAGE_SECTION_HEADER curSection = firstSection + i;
		const DWORD offsetInMemoty = curSection->VirtualAddress;
		const DWORD offsetInFile = curSection->PointerToRawData;
		const DWORD sectionSizeInFile = curSection->SizeOfRawData;
		memcpy(ImageBuffer + offsetInMemoty, buffer + offsetInFile, sectionSizeInFile);
	}
	free(buffer);
	return ImageBuffer;
}
bool ImageMemory2File(PBYTE pMemBuffer, const char* destPath) {
	const PIMAGE_DOS_HEADER const dosHeader = (PIMAGE_DOS_HEADER)pMemBuffer;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		std::println("has not dos format");
		return false;
	}
	const PIMAGE_NT_HEADERS const ntHeader = (PIMAGE_NT_HEADERS)((DWORD)pMemBuffer + dosHeader->e_lfanew);
	if (ntHeader->Signature != IMAGE_NT_SIGNATURE) {
		std::println("Invalid NT signature");
		return false;
	}
	const PIMAGE_OPTIONAL_HEADER const optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)ntHeader
		+ sizeof(ntHeader->Signature)
		+ sizeof(ntHeader->FileHeader));// ntHeader->OptionalHeader;
	assert(optionalHeader == &ntHeader->OptionalHeader, "optionalHeader != ntHeader->OptionalHeader");
	DWORD fileSize = 0;
	// ��������header�ͽڱ�Ĵ�С
	fileSize += optionalHeader->SizeOfHeaders;
	//	����ÿһ�ڵĴ�С
	const size_t sectionCount = ntHeader->FileHeader.NumberOfSections;
	const PIMAGE_SECTION_HEADER const firstSection = IMAGE_FIRST_SECTION(ntHeader);
	for (size_t i = 0; i < sectionCount; ++i) {
		const PIMAGE_SECTION_HEADER const curSection = firstSection + i;
		fileSize += curSection->SizeOfRawData;
	}
	PBYTE pFileBuffer = (PBYTE)malloc(fileSize);
	if (!pFileBuffer) {
		strerror_s(errStr, 0x20, errno);
		std::println("malloc failed, because:{}", errStr);
		return false;
	}
	memset(pFileBuffer, 0, fileSize);
	// �������е�ͷ�ͽڱ�
	memcpy(pFileBuffer, pMemBuffer, optionalHeader->SizeOfHeaders);
	for (size_t i = 0; i < sectionCount; ++i) {
		const PIMAGE_SECTION_HEADER const curSection = firstSection + i;
		const DWORD offsetInMemry = curSection->VirtualAddress;
		const DWORD offsetInFile = curSection->PointerToRawData;
		const DWORD curSectionSizeinFile = curSection->SizeOfRawData;
		PBYTE dest = pFileBuffer + offsetInFile;
		PBYTE src = pMemBuffer + offsetInMemry;
		memcpy(dest, src, curSectionSizeinFile);
	}

	FILE* fp;
	errno_t err = fopen_s(&fp, destPath, "wb");
	if (!fp) {
		strerror_s(errStr, 0x20, errno);
		std::println("fopen failed, because:{}", errStr);
		return false;
	}
	size_t ret = fwrite(pFileBuffer, fileSize, 1, fp);
	if (ret != 1) {
		strerror_s(errStr, 0x20, errno);
		std::println("fwrite failed, because:{}", errStr);
		fclose(fp);
		return false;
	}
	fclose(fp);
	free(pFileBuffer);
	return true;
}
DWORD RVA2FOA(IN LPVOID pMemoryBuffer, IN DWORD Rva)
{
	auto const dosHeader = GetDosHeader(pMemoryBuffer);
	if (dosHeader == NULL) return -1;
	auto const ntHeader = GetNTHeader(pMemoryBuffer, dosHeader);
	if (ntHeader == NULL) return -1;
	auto const optionalHeader = &ntHeader->OptionalHeader;
	// ���ƫ��������ͷ������ôֱ�ӷ��ؼ���
	if (Rva < optionalHeader->SizeOfHeaders)
	{
		return Rva;
	}
	auto const sectionCount = ntHeader->FileHeader.NumberOfSections;
	auto const firstSection = IMAGE_FIRST_SECTION(ntHeader);
	for (int i = 0; i < sectionCount; ++i)
	{
		auto const curSection = firstSection + i;
		auto const virtualAddress = curSection->VirtualAddress;
		auto const ptoRawData = curSection->PointerToRawData;
		auto const sectionSize = curSection->SizeOfRawData;
		if (Rva >= virtualAddress && Rva < virtualAddress + sectionSize)
		{
			auto const ans = ptoRawData + (Rva - virtualAddress);
			//std::println("the foa is {:X}", ans);
			return ans;
		}
	}
	return 0;
}
DWORD FOA2RVA(IN LPVOID pMemoryBuffer, IN DWORD Foa)
{
	auto const dosHeader = GetDosHeader(pMemoryBuffer);
	if (dosHeader == NULL) return -1;
	auto const ntHeader = GetNTHeader(pMemoryBuffer, dosHeader);
	if (ntHeader == NULL) return -1;
	auto const optionalHeader = &ntHeader->OptionalHeader;
	// ���ƫ��������ͷ������ôֱ�ӷ��ؼ���
	if (Foa < optionalHeader->SizeOfHeaders)
	{
		return Foa;
	}
	auto const sectionCount = ntHeader->FileHeader.NumberOfSections;
	auto const firstSection = IMAGE_FIRST_SECTION(ntHeader);
	for (int i = 0; i < sectionCount; ++i)
	{
		auto const curSection = firstSection + i;
		auto const virtualAddress = curSection->VirtualAddress;
		auto const ptoRawData = curSection->PointerToRawData;
		auto const sectionSize = curSection->SizeOfRawData;
		if (Foa >= ptoRawData && Foa < ptoRawData + sectionSize)
		{
			auto const ans = virtualAddress + (Foa - ptoRawData);
			//std::println("the rva is {:X}", ans);
			return ans;
		}
	}
	return 0;
}
// д���ڴ��е����ݵ���Ӧ�ļ���
bool write_file(IN void* buffer, IN const char* filename, IN const DWORD size)
{
	FILE* fp;
	errno_t err = fopen_s(&fp, filename, "wb");
	auto ans = fwrite(buffer, size, 1, fp);
	if (ans == -1)
	{
		strerror_s(errStr, 0x20, errno);
		std::println("fwrite fail, becase:", errStr);
		return false;
	}
	fclose(fp);
	return true;
}

DWORD Offset(PVOID buffer, PVOID addr)
{
	return (DWORD)(addr)-(DWORD)buffer;
}

void* GetBufferAddr(PVOID buffer, DWORD rva)
{
	DWORD foa = RVA2FOA(buffer, rva);
	return (PVOID)((PBYTE)buffer + foa);
}


/*
	����һ���ڣ���СΪ0x1000 �ֽڵĽڣ�֮�󷵻������ڵ�FOA
*/
bool AddNewSection(IN  LPCTSTR infilename, int& newFOA)
{
	/*
		���ļ���ȡ���ڴ�����
		�ж��Ƿ��ܹ��ڲ�����SizeOfHeaders����������һ���ڱ�
		����һ���µ��㹻����ڴ��������µ�memoryImage
		����ԭ����memoryImage���µ���
		���һ����
		���һ���ڱ����Ը���һ���ڱ�
		�޸Ľڱ��еĽڴ�С������󣩣�va��misc.VirtualSize������ǰ����foa
		�޸�NumberOfSection
		�޸�SizeOfImage
		д�ص��ļ���
	*/
	void* memoryImage = ReadMemoryImage(infilename);
	if (memoryImage == NULL)
	{
		std::println("���ڴ澵��buffer�ļ�ʧ��");
		return false;
	}
	PIMAGE_DOS_HEADER dosHeader = GetDosHeader(memoryImage);
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(memoryImage, dosHeader);
	PIMAGE_FILE_HEADER fileHader = &ntHeaders->FileHeader;
	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;
	// �ж�headers�Ŀ��д�С�Ƿ񻹷���Ҫ��
	const int freebytes = opHeader->SizeOfHeaders - (dosHeader->e_lfanew +
		sizeof(IMAGE_NT_HEADERS) + fileHader->NumberOfSections * sizeof(IMAGE_SECTION_HEADER));
	if (freebytes < 2 * sizeof(IMAGE_SECTION_HEADER))
	{
		std::println("���ļ���headers���������޷�����һ�Žڱ�");
		return false;
	}
	// ��������һ�ں��ImageSize,��������0x1000����
	const int newSizeOfImage = opHeader->SizeOfImage + 0x1000;
	void* newMemoryImage = calloc(1, newSizeOfImage);
	assert(newMemoryImage);
	memcpy(newMemoryImage, memoryImage, opHeader->SizeOfImage);
	free(memoryImage);
	dosHeader = GetDosHeader(newMemoryImage);
	ntHeaders = GetNTHeader(newMemoryImage, dosHeader);
	fileHader = &ntHeaders->FileHeader;
	opHeader = &ntHeaders->OptionalHeader;
	// ���Ƶ�һ���ڱ����
	PIMAGE_SECTION_HEADER firSection = IMAGE_FIRST_SECTION(ntHeaders);
	const int sectionCount = fileHader->NumberOfSections;
	memcpy(firSection + sectionCount, firSection, sizeof(IMAGE_SECTION_HEADER));
	memset(firSection + sectionCount + 1, 0, sizeof(IMAGE_SECTION_HEADER));
	PIMAGE_SECTION_HEADER curSection = firSection + sectionCount;
	const char* name = "export";
	memset(curSection->Name, 0, sizeof(curSection->Name));
	memcpy(curSection->Name, name, strlen(name));
	curSection->Misc.VirtualSize = 0x1000;
	curSection->SizeOfRawData = 0x1000;
	// �������һҳ�Ĵ�С��ÿһҳ�Ĵ�С������SectionALignment�������������ܹ�ȫ����SizeOfRawData��VirtualSize
	PIMAGE_SECTION_HEADER preSection = curSection - 1;
	DWORD sectionSize = (preSection->SizeOfRawData) > (preSection->Misc.VirtualSize) ? (preSection->SizeOfRawData) : (preSection->Misc.VirtualSize);
	if (sectionSize % opHeader->SectionAlignment != 0)
	{
		sectionSize = (sectionSize / opHeader->SectionAlignment + 1) * opHeader->SectionAlignment;
	}
	curSection->VirtualAddress = preSection->VirtualAddress + sectionSize;
	//�������ļ��е�ƫ�ƣ�����ȽϹ̶�����ΪSizeOfRawData�������Ƕ����Ĵ�С
	curSection->PointerToRawData = preSection->PointerToRawData + preSection->SizeOfRawData;
	// �޸�����ֵ
	newFOA = curSection->PointerToRawData;
	// �޸Ľ�����
	fileHader->NumberOfSections = fileHader->NumberOfSections + 1;
	// �޸��ڴ澵���С
	opHeader->SizeOfImage = newSizeOfImage;
	auto ans = ImageMemory2File((PBYTE)newMemoryImage, infilename);
	free(newMemoryImage);
	return ans;

}
bool BigerSection(IN LPCTSTR infilename, IN const char* outfilename)
{
	/*
		��ȡ���ڴ���
		������½��ں�memoryImage�Ĵ�С
		���������ڴ沢�ҿ���ԭ�������ݵ��µ��ڴ���
		������һ�����µĿ�������
		�޸Ľڱ�
		�޸Ľڱ�����
		�޸�SizeOfImage
	*/
	void* memoryImage = ReadMemoryImage(infilename);
	assert(memoryImage);
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(memoryImage, GetDosHeader(memoryImage));
	int const imageSize = ntHeaders->OptionalHeader.SizeOfImage + ntHeaders->OptionalHeader.SectionAlignment;
	void* newMemoryImage = calloc(1, imageSize);
	assert(newMemoryImage);
	memcpy(newMemoryImage, memoryImage, ntHeaders->OptionalHeader.SizeOfImage);
	free(memoryImage);

	PIMAGE_DOS_HEADER dosHeader = GetDosHeader(newMemoryImage);
	ntHeaders = GetNTHeader(newMemoryImage, dosHeader);
	PIMAGE_FILE_HEADER fileHeader = &ntHeaders->FileHeader;
	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;

	int const sectionCount = fileHeader->NumberOfSections;
	PIMAGE_SECTION_HEADER lastSection = IMAGE_FIRST_SECTION(ntHeaders) + (sectionCount - 1);
	lastSection->Misc.VirtualSize = imageSize - lastSection->VirtualAddress;
	lastSection->SizeOfRawData = lastSection->Misc.VirtualSize;
	opHeader->SizeOfImage = imageSize;
	ImageMemory2File((PBYTE)newMemoryImage, outfilename);
	return true;
}
std::string RelocatedTable(PVOID pFileBuffer)
{
	/*
	�ҵ��ض����
	��ӡ��Ӧ�Ľṹ
	*/
	std::string output;
	PIMAGE_DOS_HEADER dosHeader = GetDosHeader(pFileBuffer);
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(pFileBuffer, dosHeader);
	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;
	// �ض�����rva
	DWORD relocate_table_rva = opHeader->DataDirectory[5].VirtualAddress;
	if (relocate_table_rva == 0)
	{
		return output;
	}
	// �ض�����foa
	DWORD relocate_table_foa = RVA2FOA(pFileBuffer, relocate_table_rva);
	// �ض����ĵ�һ��ָ��
	PIMAGE_BASE_RELOCATION first_relocation_table = (PIMAGE_BASE_RELOCATION)((DWORD)pFileBuffer + relocate_table_foa),
		cur_table = first_relocation_table;


	//ѭ�����е��ض����
	int i = 0;
	while (cur_table->SizeOfBlock != 0)
	{
		++i;
		DWORD base = cur_table->VirtualAddress, size = cur_table->SizeOfBlock;
		int item_count = (size - 8) / 2;
		output += std::format("��{}�飬��ַΪ0x{:X},���СΪ{} bytes����������Ϊ{}\n", i, base, size, item_count);
		PWORD pItem = (PWORD)((PBYTE)cur_table + 8);
		WORD first4 = 0xf000, last12 = 0x0fff;
		for (int j = 0; j < item_count; ++j)
		{
			WORD value = pItem[j];
			// ǰ4bit
			int type = (value & first4) >> 12;
			// ��12bit
			WORD rva_offset = value & last12;
			if (type != 0)
			{
				output += std::format("��{}�������{}��rva��0x{:X}\n", j + 1, type, base + rva_offset);
			}
			else {
				output += std::format("��{}�������{}, rva: N/A\n", j + 1, type);
			}
		}
		cur_table = (PIMAGE_BASE_RELOCATION)((DWORD)cur_table + size);
	}
	free(pFileBuffer);
	return output;
}
std::string ExportTable(PVOID fileImage)
{
	/*
	�ҵ�������
	�ҵ����ֱ�
	��ӡ��������
	*/
	std::string output;
	PIMAGE_DOS_HEADER dosHeader = GetDosHeader(fileImage);
	if (dosHeader == NULL)
	{
		output += std::format("dosHeader is invalid\n");
		return output;;
	}
	PIMAGE_NT_HEADERS ntHeaders = GetNTHeader(fileImage, dosHeader);
	PIMAGE_OPTIONAL_HEADER opHeader = &ntHeaders->OptionalHeader;
	// �������ǵ�һ�ű�
	IMAGE_DATA_DIRECTORY exportDataDict = opHeader->DataDirectory[0];
	// �������RVA
	DWORD exportTable_rva = exportDataDict.VirtualAddress;
	if (exportTable_rva == 0)
	{
		return output;
	}
	// �������FOA
	DWORD exportTable_foa = RVA2FOA(fileImage, exportTable_rva);
	// �������ָ��
	PIMAGE_EXPORT_DIRECTORY pExporttable = (PIMAGE_EXPORT_DIRECTORY)((DWORD)fileImage + exportTable_foa);
	// ��ӡһЩ��Ϣ
	DWORD name_foa = RVA2FOA(fileImage, pExporttable->Name);
	const char* name = (const char*)((DWORD)fileImage + name_foa);
	output += std::format("Name:{}\nBase:{}\nNumberOfFunctions:{}\nNumberOfNames:{}\n------------\n",
		name, pExporttable->Base, pExporttable->NumberOfFunctions, pExporttable->NumberOfNames);

	//������ĺ������ֱ��RVA
	DWORD nameTable_rva = pExporttable->AddressOfNames;
	// ������ĺ������ֱ��FOA
	DWORD nameTable_foa = RVA2FOA(fileImage, nameTable_rva);
	// �������ֱ��ָ��
	PDWORD pNameTable = (PDWORD)((DWORD)fileImage + nameTable_foa);

	// ordinal���rva
	DWORD ordinal_table_rva = pExporttable->AddressOfNameOrdinals;
	// ordinal���foa
	DWORD ordinal_table_foa = RVA2FOA(fileImage, ordinal_table_rva);
	// ordinal��
	PWORD ordinal_table = (PWORD)((DWORD)fileImage + ordinal_table_foa);
	// ��ӡordinals���names��
	output += std::format("functions' name and ordinals\n");
	for (DWORD i = 0; i < pExporttable->NumberOfNames; ++i)
	{
		// ��������rva
		DWORD funcName_rva = pNameTable[i];
		// �������ֵ�foa
		DWORD funcName_foa = RVA2FOA(fileImage, funcName_rva);
		//��ӡ������
		const char* funcName = (const char*)((DWORD)fileImage + funcName_foa);
		output += std::format("ordinalΪ{}�ĺ�����Ϊ{}, foa is {:X}\n",
			(ordinal_table[i]), funcName, funcName_foa);
	}
	DWORD func_table_rva = pExporttable->AddressOfFunctions;
	DWORD func_table_foa = RVA2FOA(fileImage, func_table_rva);
	PDWORD func_table = (PDWORD)((DWORD)fileImage + func_table_foa);
	output += std::format("functions' address \n");
	for (DWORD i = 0; i < pExporttable->NumberOfFunctions; ++i)
	{
		output += std::format("�±�Ϊ{}�ĺ�����rva��{:X}\n", i, func_table[i]);
	}
	free(fileImage);
	return output;
}
std::string ImportTable(PVOID file_buffer)
{
	/*
	�ҵ������
	�ҵ�����dll
	��ӡ������Ա
	*/
	// ������rva
	std::string output;
	PIMAGE_DOS_HEADER dos_header = GetDosHeader(file_buffer);
	PIMAGE_NT_HEADERS nt_headers = GetNTHeader(file_buffer, dos_header);
	PIMAGE_OPTIONAL_HEADER op_headers = &nt_headers->OptionalHeader;

	DWORD import_table_rva = op_headers->DataDirectory[1].VirtualAddress;
	DWORD import_table_foa = RVA2FOA(file_buffer, import_table_rva);
	PIMAGE_IMPORT_DESCRIPTOR first_import_table = (PIMAGE_IMPORT_DESCRIPTOR)((PBYTE)file_buffer + import_table_foa);
	PIMAGE_IMPORT_DESCRIPTOR cur_import_table = first_import_table;

	while (cur_import_table->Name != 0)
	{
		// ���dll����
		DWORD dll_name_rva = cur_import_table->Name;
		DWORD dll_name_foa = RVA2FOA(file_buffer, dll_name_rva);
		const char* dll_name = (PCSTR)((PBYTE)file_buffer + dll_name_foa);
		output += std::format("------ now dll name is {} -------\n", dll_name);
		// û�а�ʱ�����
		//����IAT
		PDWORD cur_INT = (PDWORD)GetBufferAddr(file_buffer, cur_import_table->OriginalFirstThunk);
		constexpr DWORD first_1 = 1 << 31, last_31 = first_1 - 1;
		output += std::format("----INT ������----\n");
		while (*cur_INT != 0)
		{
			DWORD value = *cur_INT;
			if ((value & first_1) == first_1)
			{
				// �������
				output += std::format("INT���иú������Ϊ{:X}\n", value & last_31);
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME import_name = (PIMAGE_IMPORT_BY_NAME)GetBufferAddr(file_buffer, value);
				PCSTR name = (PCSTR)import_name->Name;
				output += std::format("INT���иú�����Ϊhit/name:0x{:X}---{}\n", import_name->Hint, name);
			}
			cur_INT++;
		}
		// ����FirstThunkkk
		if (cur_import_table->TimeDateStamp == -1)
		{
			output += std::format("����һ���Ѿ��󶨵�IAT��,����\n");
			continue;
		}
		output += std::format("----IAT ������----\n");
		PDWORD cur_IAT = (PDWORD)GetBufferAddr(file_buffer, cur_import_table->FirstThunk);
		while (*cur_IAT != 0)
		{
			DWORD value = *cur_IAT;
			if ((value & first_1) == first_1)
			{
				output += std::format("IAT��ú������Ϊ{:X}\n", value & last_31);
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME import_name = (PIMAGE_IMPORT_BY_NAME)GetBufferAddr(file_buffer, value);
				PCSTR name = (PCSTR)import_name->Name;
				output += std::format("IAT���иú���������Ϊhit/name:0x{:X}---{}\n", import_name->Hint, name);
			}
			cur_IAT++;
		}
		cur_import_table++;
	}
	free(file_buffer);
	return output;
}
std::string BoundImportTable(PVOID file_buffer)
{
	std::string output;
	PIMAGE_DOS_HEADER dos_header = GetDosHeader(file_buffer);
	PIMAGE_NT_HEADERS nt_headers = GetNTHeader(file_buffer, dos_header);
	PIMAGE_OPTIONAL_HEADER op_header = &nt_headers->OptionalHeader;
	if (op_header->DataDirectory[11].VirtualAddress == 0)
	{
		output += std::format("��PE�ļ�û�а󶨵����\n");
		return output;
	}
	PIMAGE_BOUND_IMPORT_DESCRIPTOR first_bound_import_desc = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)GetBufferAddr(file_buffer,
		op_header->DataDirectory[11].VirtualAddress);
	PIMAGE_BOUND_IMPORT_DESCRIPTOR cur_bound_import_desc = first_bound_import_desc;
	if (cur_bound_import_desc == NULL) return output;

	int index = 0;
	while (cur_bound_import_desc->OffsetModuleName != 0 && cur_bound_import_desc->TimeDateStamp != 0)
	{
		// ��ӡ���ֺ�ʱ���
		index = 0;
		PCSTR dll_name = (PCSTR)((PBYTE)first_bound_import_desc + cur_bound_import_desc->OffsetModuleName);
		output += std::format("---��dll��ʱ���Ϊ{:X}, ������Ϊ{}\n", cur_bound_import_desc->TimeDateStamp, dll_name);

		// ��ӡ���������ֺ�ʱ���
		if (cur_bound_import_desc->NumberOfModuleForwarderRefs > 0)
		{
			output += std::format("��dll������dll��{}�������£�\n", cur_bound_import_desc->NumberOfModuleForwarderRefs);
			for (int i = 0; i < cur_bound_import_desc->NumberOfModuleForwarderRefs; ++i)
			{
				++index;
				PIMAGE_BOUND_FORWARDER_REF bound_forwarder = (PIMAGE_BOUND_FORWARDER_REF)(cur_bound_import_desc + 1 + i);
				dll_name = (PCSTR)((PBYTE)first_bound_import_desc + bound_forwarder->OffsetModuleName);
				output += std::format("����{}---��dll��ʱ���Ϊ{:X}, ������Ϊ{}\n", i + 1, bound_forwarder->TimeDateStamp, dll_name);
			}
		}
		else
		{
			output += std::format("��dllû��������dll\n");
		}

		++index;
		// �ƶ�����һ���󶨵���������
		cur_bound_import_desc += index;
	}
	free(file_buffer);
	return output;
}
std::string PrintResourceDirectory(PIMAGE_RESOURCE_DIRECTORY res_directory, void* file_buffer, int level, void* res_addr)
{
	std::string output;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY res_entry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)
		((PBYTE)res_directory + sizeof(IMAGE_RESOURCE_DIRECTORY));
	int count = res_directory->NumberOfIdEntries + res_directory->NumberOfNamedEntries;

	for (int i = 0; i < count; ++i)
	{
		PIMAGE_RESOURCE_DIRECTORY_ENTRY cur_entry = res_entry + i;
		for (int j = 0; j < level; ++j)output += std::format("   ");  // Indent based on level
		if (cur_entry->NameIsString)
		{
			// Handle named entries if necessary
			output += std::format("Name: {}\n", (PCHAR)(GetBufferAddr(file_buffer, cur_entry->Name)));
		}
		else
		{
			//cout << "ID: " << cur_entry->Name << endl;
			output += std::format("ID: {}\n", cur_entry->Name);
		}

		if (cur_entry->DataIsDirectory)
		{
			PIMAGE_RESOURCE_DIRECTORY sub_directory = (PIMAGE_RESOURCE_DIRECTORY)
				((PBYTE)res_addr + (cur_entry->OffsetToDirectory & 0x7FFFFFFF));
			output += PrintResourceDirectory(sub_directory, file_buffer, level + 1, res_addr);
		}
		else
		{
			PIMAGE_RESOURCE_DATA_ENTRY data_entry = (PIMAGE_RESOURCE_DATA_ENTRY)
				((PBYTE)res_addr + cur_entry->OffsetToData);
			for (int j = 0; j < level; ++j)output += std::format("   "); // Indent based on level
			output += std::format("RVA is 0x{:X}, size is 0x{:X}\n", data_entry->OffsetToData,
				data_entry->Size);
		}
	}
	return output;
}
std::string ResourceTable(PVOID file_buffer)
{
	/* �ҵ���Դ���׵�ַ
	����type�ҵ���һ����Ҫ��������
	�ҵ����еڶ�������ӡ��Դ���
	�ҵ����������е����ӡ����ҳ
	���ݵ������ҵ�rva��size
	*/
	PIMAGE_DOS_HEADER dos_header = GetDosHeader(file_buffer);
	PIMAGE_NT_HEADERS nt_headers = GetNTHeader(file_buffer, dos_header);
	PIMAGE_OPTIONAL_HEADER op_header = &nt_headers->OptionalHeader;
	// ��һ��
	PIMAGE_RESOURCE_DIRECTORY res_directory = (PIMAGE_RESOURCE_DIRECTORY)GetBufferAddr(file_buffer,
		op_header->DataDirectory[2].VirtualAddress);
	// ���type��-1����ӡ���е�����
	std::string ans = PrintResourceDirectory(res_directory, file_buffer, 0, res_directory);
	return ans;
}
