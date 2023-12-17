#include <sstream>
#include <iomanip>

#include "Z80Disassembly.h"
#include "Z80Opcodes.h"


TExternalDataTypeConverter* TZ80Disassembly::Create() {

    return new TZ80Disassembly();

}

TZ80Disassembly::TZ80Disassembly() {

    FTypeName = L"{s:Disassembly(\"Z80\")}";
    FFriendlyTypeName = FTypeName;
    FWidth = dtwVariable;
    FMaxTypeSize = 4;
    FSupportedByteOrders = boLittleEndian;
    FSupportsStrToBytes = FALSE;

}

void TZ80Disassembly::ChangeByteOrder(uint8_t* Bytes, int ByteCount, TByteOrder TargetByteOrder) {
    
}

TBytesToStrError TZ80Disassembly::BytesToStr(uint8_t* Bytes, int ByteCount, TIntegerDisplayOption IntegerDisplayOption, 
    int& ConvertedByteCount, std::wstring& ConvertedStr) {

    if (ByteCount < 1) return btseBytesTooShort;

    uint32_t opcode = 0;

    for (int bytePos = 0; bytePos < 3; bytePos++) {

        opcode = (opcode << 8) | Bytes[bytePos == 2 ? 3 : bytePos];

        if (opcodes.find(opcode) != opcodes.end()) {

            ConvertedStr = opcodes[opcode];
            ConvertedByteCount = bytePos + 1;

            if ((ConvertedStr.find(L"rst") != ConvertedStr.npos) && (IntegerDisplayOption != idoDecimal)) {

                std::wostringstream oss;

                oss << L'$' << std::hex;
                oss << (IntegerDisplayOption == idoHexadecimalUpperCase ? std::uppercase : std::nouppercase);
                oss << std::setw(2) << std::setfill(L'0');

                oss << (Bytes[0] - 0xC7);

                ConvertedStr.replace(4, 1, oss.str());

                return btseNone;

            }

            if (ConvertedStr.find(L"+*") != ConvertedStr.npos) {

                ConvertedByteCount++;

                if (Bytes[2] != 0) {

                    std::wostringstream oss;

                    oss << (Bytes[2] > 127 ? L"-" : L"+");

                    switch (IntegerDisplayOption) {
                        case idoDecimal:
                            oss << std::dec;
                            break;
                        case idoHexadecimalUpperCase:
                            oss << L'$' << std::hex << std::uppercase << std::setw(2) << std::setfill(L'0');
                            break;
                        case idoHexadecimalLowerCase:
                            oss << L'$' << std::hex << std::nouppercase << std::setw(2) << std::setfill(L'0');
                            break;
                    }

                    oss << (Bytes[2] > 127 ? abs(Bytes[2] - 256) : Bytes[2]);

                    ConvertedStr.replace(ConvertedStr.find(L"+*"), 2, oss.str());

                }

                else ConvertedStr.erase(ConvertedStr.find(L"+*"), 2);

            }

            else if (ConvertedStr.find(L"**") != ConvertedStr.npos) {

                ConvertedByteCount += 2;

                std::wostringstream oss;

                switch (IntegerDisplayOption) {
                    case idoDecimal:
                        oss << std::dec;
                        break;
                    case idoHexadecimalUpperCase:
                        oss << L'$' << std::hex << std::uppercase << std::setw(4) << std::setfill(L'0');
                        break;
                    case idoHexadecimalLowerCase:
                        oss << L'$' << std::hex << std::nouppercase << std::setw(4) << std::setfill(L'0');
                        break;
                }

                oss << (Bytes[bytePos + 2] << 8 | Bytes[bytePos + 1]);

                ConvertedStr.replace(ConvertedStr.find(L"**"), 2, oss.str());

                return btseNone;

            }

            if (ConvertedStr.find(L"*") != ConvertedStr.npos) {

                ConvertedByteCount++;

                std::wostringstream oss;

                if ((ConvertedStr.find(L"jr") != ConvertedStr.npos) || (ConvertedStr.find(L"djnz") != ConvertedStr.npos))
                    oss << (Bytes[2] > 127 ? L"-" : L"+");

                switch (IntegerDisplayOption) {
                    case idoDecimal:
                        oss << std::dec;
                        break;
                    case idoHexadecimalUpperCase:
                        oss << L'$' << std::hex << std::uppercase << std::setw(2) << std::setfill(L'0');
                        break;
                    case idoHexadecimalLowerCase:
                        oss << L'$' << std::hex << std::nouppercase << std::setw(2) << std::setfill(L'0');
                        break;
                }

                if ((ConvertedStr.find(L"jr") != ConvertedStr.npos) || (ConvertedStr.find(L"djnz") != ConvertedStr.npos))
                    oss << (Bytes[2] > 127 ? abs(Bytes[2] - 254) : (Bytes[2] + 2));

                else oss << Bytes[bytePos + 1];

                ConvertedStr.replace(ConvertedStr.find(L"*"), 1, oss.str());

            }

            return btseNone;

        }

    }

    ConvertedStr = L"";

    ConvertedByteCount = 1;

    return btseInvalidBytes;

}

TStrToBytesError TZ80Disassembly::StrToBytes(std::wstring Str, TIntegerDisplayOption IntegerDisplayOption,
    std::vector<uint8_t>& ConvertedBytes) {

    return stbeInvalidString;

}
