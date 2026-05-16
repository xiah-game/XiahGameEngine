import sys

with open('d:/xiahold/XiahGameEngine/EffectRes.cpp', 'rb') as f:
    data = f.read()

# Replace IsBadReadPtr with simple address check to avoid first-chance exceptions in VS
target1 = b'if (pEffect == NULL || IsBadReadPtr(pEffect, sizeof(_EFFECT))) return NULL;'
replacement1 = b'if (pEffect == NULL || (DWORD_PTR)pEffect < 0x10000) return NULL;'
data = data.replace(target1, replacement1)

target2 = b'if( pEffect == NULL || IsBadReadPtr(pEffect, sizeof(_EFFECT)) )'
replacement2 = b'if( pEffect == NULL || (DWORD_PTR)pEffect < 0x10000 )'
data = data.replace(target2, replacement2)

with open('d:/xiahold/XiahGameEngine/EffectRes.cpp', 'wb') as f:
    f.write(data)

print("Replaced IsBadReadPtr successfully.")
