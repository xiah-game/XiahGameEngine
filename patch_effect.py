with open('d:/xiahold/XiahGameEngine/EffectRes.cpp', 'rb') as f:
    data = f.read()

target = b'XIAHGE_API _EFFECTPACKAGE* CEffect::EnqEffectImmediately(_EFFECT* pEffect, int nSTime, int nX, int nY, int nZ, int nBoneIndex)\r\n{'
replacement = b'XIAHGE_API _EFFECTPACKAGE* CEffect::EnqEffectImmediately(_EFFECT* pEffect, int nSTime, int nX, int nY, int nZ, int nBoneIndex)\r\n{\r\n\tif (pEffect == NULL) return NULL;\r\n'

if target in data:
    data = data.replace(target, replacement)
    with open('d:/xiahold/XiahGameEngine/EffectRes.cpp', 'wb') as f:
        f.write(data)
    print("Patched successfully.")
else:
    # Try finding it with \n instead of \r\n
    target2 = b'XIAHGE_API _EFFECTPACKAGE* CEffect::EnqEffectImmediately(_EFFECT* pEffect, int nSTime, int nX, int nY, int nZ, int nBoneIndex)\n{'
    replacement2 = b'XIAHGE_API _EFFECTPACKAGE* CEffect::EnqEffectImmediately(_EFFECT* pEffect, int nSTime, int nX, int nY, int nZ, int nBoneIndex)\n{\n\tif (pEffect == NULL) return NULL;\n'
    if target2 in data:
        data = data.replace(target2, replacement2)
        with open('d:/xiahold/XiahGameEngine/EffectRes.cpp', 'wb') as f:
            f.write(data)
        print("Patched successfully (LF format).")
    else:
        print("Target not found.")
