import sys

with open('d:/xiahold/XiahGameEngine/EffectRes.cpp', 'rb') as f:
    data = f.read()

# Patch EnqEffectImmediately
target1 = b'XIAHGE_API _EFFECTPACKAGE* CEffect::EnqEffectImmediately(_EFFECT* pEffect, int nSTime, int nX, int nY, int nZ, int nBoneIndex)\r\n{\r\nif (pEffect == NULL) return NULL;'
replacement1 = b'XIAHGE_API _EFFECTPACKAGE* CEffect::EnqEffectImmediately(_EFFECT* pEffect, int nSTime, int nX, int nY, int nZ, int nBoneIndex)\r\n{\r\nif (pEffect == NULL || IsBadReadPtr(pEffect, sizeof(_EFFECT))) return NULL;'

if target1 in data:
    data = data.replace(target1, replacement1)
else:
    target1_lf = b'XIAHGE_API _EFFECTPACKAGE* CEffect::EnqEffectImmediately(_EFFECT* pEffect, int nSTime, int nX, int nY, int nZ, int nBoneIndex)\n{\nif (pEffect == NULL) return NULL;'
    replacement1_lf = b'XIAHGE_API _EFFECTPACKAGE* CEffect::EnqEffectImmediately(_EFFECT* pEffect, int nSTime, int nX, int nY, int nZ, int nBoneIndex)\n{\nif (pEffect == NULL || IsBadReadPtr(pEffect, sizeof(_EFFECT))) return NULL;'
    if target1_lf in data:
        data = data.replace(target1_lf, replacement1_lf)

# Patch EnqHitEffectImmediately
target2 = b'_EFFECT* pEffect = m_pHitEffect[nType];\r\n\tif( pEffect == NULL )'
replacement2 = b'_EFFECT* pEffect = m_pHitEffect[nType];\r\n\tif( pEffect == NULL || IsBadReadPtr(pEffect, sizeof(_EFFECT)) )'

if target2 in data:
    data = data.replace(target2, replacement2)
else:
    target2_lf = b'_EFFECT* pEffect = m_pHitEffect[nType];\n\tif( pEffect == NULL )'
    replacement2_lf = b'_EFFECT* pEffect = m_pHitEffect[nType];\n\tif( pEffect == NULL || IsBadReadPtr(pEffect, sizeof(_EFFECT)) )'
    if target2_lf in data:
        data = data.replace(target2_lf, replacement2_lf)

with open('d:/xiahold/XiahGameEngine/EffectRes.cpp', 'wb') as f:
    f.write(data)

print("Patched with IsBadReadPtr successfully.")
