#include "stdafx.h"
#include "HvxCalls.h"
#pragma warning(push)
#pragma warning(disable:4826) // Get rid of the sign-extended warning

#define HvxCall QWORD _declspec(naked)

namespace Hvx
{
	// lets hide stuff the rest of the project doesn't need to see
	namespace
	{
		const WORD cMagic = 0x4D4D;
		const WORD cVersion = 0x4099;
		const DWORD cFlags = NULL;
		const DWORD cHeaderSize = 0x120;
		const BYTE cKey[0x10] = { 0x1b, 0x9d, 0xc0, 0x98, 0xd8, 0x09, 0x81, 0x50, 0xe5, 0x57, 0xc2, 0xc2, 0xd8, 0xfe, 0xf9, 0xf7 }; // https://www.random.org/bytes/
		const BYTE cSig[0x100] = { NULL };
		const DWORD cBufferSize = 0x3000;
		
		typedef struct _KeysExecute
		{
			WORD Magic;			// 0 : 2
			WORD Version;		// 2 : 2
			DWORD Flags;		// 4 : 4
			DWORD EntryPoint;	// 8 : 4
			DWORD Size;			// 0xC : 4
			BYTE key[0x10];		// 0x10 : 0x10
			BYTE Sig[0x100];	// 0x20 : 0x100
			// Header: 0x120
		}KeysExecutes, *PKeysExecute;

		const BYTE HvProcExp[] = {
			0x48, 0x58, 0x50, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x5F, 0xF5, 0xD4, 0x2B,
			0x17, 0xE7, 0xFA, 0x8A, 0x20, 0x18, 0x38, 0x4E, 0xF0, 0x1E, 0x9F, 0x87, 0x85, 0xA8, 0xD5, 0xB7,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x7A, 0x02, 0x59, 0x43, 0x9E, 0xE6, 0x93, 0xD5, 0x01, 0xC9, 0x48, 0x4D, 0xB2, 0xBF, 0x9D, 0x18,
			0xA9, 0x16, 0x5E, 0xFF, 0x1E, 0xD5, 0xB6, 0xA8, 0x79, 0x60, 0xA8, 0x2F, 0xC1, 0x8D, 0x20, 0x8A,
			0xEB, 0x46, 0xC3, 0x01, 0xEC, 0xC4, 0xDB, 0xDF, 0xA1, 0x04, 0xD1, 0xDF, 0x23, 0x69, 0x5E, 0xCC,
			0x50, 0xC3, 0xF4, 0xDD, 0xA4, 0x80, 0x7D, 0x05, 0x2D, 0x57, 0xFF, 0x60, 0xA5, 0x58, 0x69, 0x27,
			0x9A, 0x33, 0x70, 0xE8, 0xC2, 0x82, 0xDC, 0xDA, 0xE2, 0x4D, 0xE1, 0xF8, 0xA1, 0xD2, 0xCC, 0x8D,
			0x98, 0x05, 0xD1, 0xA3, 0x2E, 0x37, 0x06, 0x0F, 0xF7, 0xEF, 0x70, 0xEF, 0x28, 0xD7, 0xAE, 0xD5,
			0x5B, 0x19, 0x1B, 0x13, 0x50, 0x0D, 0x15, 0x00, 0x92, 0x01, 0xEE, 0x38, 0xA7, 0x61, 0xB1, 0xA1,
			0x27, 0xBF, 0xB0, 0x42, 0x0F, 0x07, 0xC3, 0x8F, 0x78, 0xAD, 0xAF, 0x87, 0x8F, 0x34, 0x8A, 0xA6,
			0x69, 0x7D, 0x40, 0xB6, 0x5C, 0x9E, 0x01, 0xEC, 0x9F, 0x5E, 0x78, 0x30, 0x09, 0x5A, 0x21, 0xFF,
			0x35, 0x8C, 0x13, 0xBE, 0xBC, 0x92, 0x67, 0xE3, 0x17, 0x0B, 0x09, 0x1C, 0xE2, 0x9D, 0xEC, 0xFD,
			0xFB, 0x6C, 0x49, 0x3A, 0xCC, 0xE7, 0x99, 0xB6, 0xB3, 0x8A, 0x8F, 0xEF, 0xF7, 0xA0, 0x28, 0x4F,
			0x72, 0xC7, 0x3D, 0xD7, 0xCC, 0xEB, 0xB2, 0x1B, 0x74, 0x93, 0xD7, 0x02, 0x05, 0xD5, 0xE1, 0x25,
			0x35, 0xD7, 0xE0, 0x50, 0xEA, 0xCF, 0x82, 0x69, 0xE3, 0x3B, 0x6F, 0xFA, 0x13, 0xF0, 0x5B, 0xD9,
			0x1A, 0xB0, 0xD3, 0x0B, 0x85, 0x4D, 0x6A, 0xFC, 0x31, 0x45, 0xC3, 0xA3, 0xFA, 0x7E, 0xDA, 0x93,
			0x84, 0xD4, 0xE7, 0xFA, 0xAB, 0x7C, 0x22, 0x96, 0x54, 0x69, 0x7F, 0xC9, 0xF8, 0xC4, 0x5A, 0xC2,
			0x82, 0x99, 0x34, 0x46, 0x81, 0x6F, 0x06, 0x33, 0x19, 0x94, 0x74, 0xA4, 0x76, 0x02, 0x4F, 0xB9,
			0x48, 0x56, 0x50, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x60, 0x00, 0x00, 0x01, 0x20,
			0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00,
			0x7D, 0x88, 0x02, 0xA6, 0xF9, 0x81, 0xFF, 0xF8, 0xFB, 0xE1, 0xFF, 0xF0, 0xFB, 0xC1, 0xFF, 0xE8,
			0xFB, 0xA1, 0xFF, 0xE0, 0xFB, 0x81, 0xFF, 0xD8, 0xFB, 0x61, 0xFF, 0xD0, 0xFB, 0x41, 0xFF, 0xC8,
			0xFB, 0x21, 0xFF, 0xC0, 0xFB, 0x01, 0xFF, 0xB8, 0xFA, 0xE1, 0xFF, 0xB0, 0xFA, 0xC1, 0xFF, 0xA8,
			0xF8, 0x21, 0xFF, 0x11, 0x7C, 0x9F, 0x23, 0x78, 0x28, 0x05, 0x00, 0x08, 0x41, 0x81, 0x00, 0x9C,
			0x28, 0x05, 0x00, 0x00, 0x41, 0x82, 0x00, 0x8C, 0x7C, 0xBD, 0x2B, 0x78, 0x3C, 0xE0, 0x80, 0x00,
			0x78, 0xE7, 0x07, 0xC6, 0x7C, 0xDE, 0x3B, 0x78, 0x3B, 0xDE, 0xFF, 0xF8, 0xE8, 0x7E, 0x00, 0x09,
			0x3B, 0xBD, 0xFF, 0xFF, 0x28, 0x1D, 0x00, 0x00, 0x41, 0x82, 0x00, 0x68, 0xE8, 0x9E, 0x00, 0x09,
			0x3B, 0xBD, 0xFF, 0xFF, 0x28, 0x1D, 0x00, 0x00, 0x41, 0x82, 0x00, 0x58, 0xE8, 0xBE, 0x00, 0x09,
			0x3B, 0xBD, 0xFF, 0xFF, 0x28, 0x1D, 0x00, 0x00, 0x41, 0x82, 0x00, 0x48, 0xE8, 0xDE, 0x00, 0x09,
			0x3B, 0xBD, 0xFF, 0xFF, 0x28, 0x1D, 0x00, 0x00, 0x41, 0x82, 0x00, 0x38, 0xE8, 0xFE, 0x00, 0x09,
			0x3B, 0xBD, 0xFF, 0xFF, 0x28, 0x1D, 0x00, 0x00, 0x41, 0x82, 0x00, 0x28, 0xE9, 0x1E, 0x00, 0x09,
			0x3B, 0xBD, 0xFF, 0xFF, 0x28, 0x1D, 0x00, 0x00, 0x41, 0x82, 0x00, 0x18, 0xE9, 0x3E, 0x00, 0x09,
			0x3B, 0xBD, 0xFF, 0xFF, 0x28, 0x1D, 0x00, 0x00, 0x41, 0x82, 0x00, 0x08, 0xE9, 0x5E, 0x00, 0x09,
			0x7F, 0xE9, 0x03, 0xA6, 0x4E, 0x80, 0x04, 0x21, 0x38, 0x21, 0x00, 0xF0, 0xE9, 0x81, 0xFF, 0xF8,
			0xEA, 0xC1, 0xFF, 0xA8, 0xEA, 0xE1, 0xFF, 0xB0, 0xEB, 0x01, 0xFF, 0xB8, 0xEB, 0x21, 0xFF, 0xC0,
			0xEB, 0x41, 0xFF, 0xC8, 0xEB, 0x61, 0xFF, 0xD0, 0xEB, 0x81, 0xFF, 0xD8, 0xEB, 0xA1, 0xFF, 0xE0,
			0xEB, 0xC1, 0xFF, 0xE8, 0xEB, 0xE1, 0xFF, 0xF0, 0x7D, 0x88, 0x03, 0xA6, 0x4E, 0x80, 0x00, 0x20
		};
	}

	namespace SysCalls
	{
		static HvxCall HvxGetVersion(DWORD magic, DWORD mode, UINT64 dest, UINT64 src, UINT32 len, UINT64 arg_r8 = NULL)
		{
			__asm
			{
				li r0, 0 // HvxGetVersion
				sc
				blr
			}
		}

		static HvxCall HvxQuiesceProcessor(BYTE Reason)
		{
			__asm
			{
				li	r0, 0x2
				sc
				blr
			}
		}

		static HvxCall HvxPostOutput(BYTE code)
		{
			__asm
			{
				li	r0, 0xD
				sc
				blr
			}
		}
	}

	// wrappers for syscalls (for organization)
	QWORD HvGetVersion(DWORD magic, DWORD mode, UINT64 dest, UINT64 src, UINT32 len, UINT64 arg_r8)
	{
		return SysCalls::HvxGetVersion(magic, mode, dest, src, len, arg_r8);
	}

	QWORD HvQuiesceProcessor(BYTE Reason)
	{
		return SysCalls::HvxQuiesceProcessor(Reason);
	}

	QWORD HvPostOutput(BYTE code)
	{
		return SysCalls::HvxPostOutput(code);
	}

	DWORD HvProcExpID = 0x48565051;
	HRESULT InitializeHvProc()
	{
		// Allocate physcial memory for this expansion
		VOID* pPhysExp = XPhysicalAlloc(0x1000, MAXULONG_PTR, 0, PAGE_READWRITE);
		DWORD physExpAdd = (DWORD)MmGetPhysicalAddress(pPhysExp);

		// Copy over our expansion data
		ZeroMemory(pPhysExp, 0x1000);
		memcpy(pPhysExp, HvProcExp, sizeof(HvProcExp));

		/*
		printf("Hashing...\n");
		BYTE Hash[0x14];
		XeCryptSha(((PBYTE)pPhysExp + 0x130), 0xED0, 0, 0, 0, 0, Hash, 0x14);
		printf("Hash: ");
		arrPrintXln(Hash, 0x14);
		printf("\n");
		*/

		// Now we can install our expansion
		printf("ExpAdd: 0x%016llX\n", physExpAdd);
		HRESULT result = (HRESULT)HvExpansionInstall(physExpAdd, 0x1000);

		// Free our allocated data
		XPhysicalFree(pPhysExp);

		if (FAILED(result))
			printf("Expansion failed to install: %08X\n", result);

		// Return our install result
		return result;
	}

	QWORD HvProc1(QWORD pqwProcedure, DWORD cArgs, PQWORD pqwArgs)
	{
		if (!cArgs)
			return HvExpansionCall(HvProcExpID, pqwProcedure, NULL, NULL, NULL);
		else
			return HvExpansionCall(HvProcExpID, pqwProcedure, cArgs, (DWORD)MmGetPhysicalAddress(pqwArgs), NULL);
	}

	QWORD HvProc(QWORD pqwProcedure, DWORD cArgs, QWORD Arg1, QWORD Arg2, QWORD Arg3, QWORD Arg4, QWORD Arg5, QWORD Arg6, QWORD Arg7, QWORD Arg8)
	{
		if(!cArgs)
			return HvExpansionCall(HvProcExpID, pqwProcedure, NULL, NULL, NULL);
		else
		{
			if (cArgs > 8)
				return 0xC800009;
			QWORD pqwArgs[8] = { Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8 };
			return HvExpansionCall(HvProcExpID, pqwProcedure, cArgs, (DWORD)MmGetPhysicalAddress(pqwArgs), NULL);
		}
		return 0;
	}
}
#pragma warning(pop)