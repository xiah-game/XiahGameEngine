with open('d:/xiahold/XiahGameEngine/EffectRes.cpp', 'rb') as f:
    data = f.read()

target = b'XIAHGE_API _EFFECTPACKAGE* CEffect::EnqHitEffectImmediately(int nType, int nSTime, int nX, int nY, int nZ, int nBoneIndex)\r\n{\r\n\t_EFFECT* pEffect = m_pHitEffect[nType];'
replacement = b'XIAHGE_API _EFFECTPACKAGE* CEffect::EnqHitEffectImmediately(int nType, int nSTime, int nX, int nY, int nZ, int nBoneIndex)\r\n{\r\n\tif (nType < 0 || nType >= eHitEnumMax) return NULL;\r\n\t_EFFECT* pEffect = m_pHitEffect[nType];'

if target in data:
    data = data.replace(target, replacement)
    with open('d:/xiahold/XiahGameEngine/EffectRes.cpp', 'wb') as f:
        f.write(data)
    print("Patched successfully.")
else:
    target2 = b'XIAHGE_API _EFFECTPACKAGE* CEffect::EnqHitEffectImmediately(int nType, int nSTime, int nX, int nY, int nZ, int nBoneIndex)\n{\n\t_EFFECT* pEffect = m_pHitEffect[nType];'
    replacement2 = b'XIAHGE_API _EFFECTPACKAGE* CEffect::EnqHitEffectImmediately(int nType, int nSTime, int nX, int nY, int nZ, int nBoneIndex)\n{\n\tif (nType < 0 || nType >= eHitEnumMax) return NULL;\n\t_EFFECT* pEffect = m_pHitEffect[nType];'
    if target2 in data:
        data = data.replace(target2, replacement2)
        with open('d:/xiahold/XiahGameEngine/EffectRes.cpp', 'wb') as f:
            f.write(data)
        print("Patched successfully (LF format).")
    else:
        print("Target not found.")
