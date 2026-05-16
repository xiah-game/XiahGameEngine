import os

file_path = r'd:\xiahold\XiahGameEngine\EffectRes.cpp'
with open(file_path, 'r', encoding='euc-kr') as f:
    content = f.read()

target = '''XIAHGE_API void XiahGameEngine::CEffect::RealizeEffectResouce(_EFFECT* pEffect)
{
	if( !pEffect->m_bLoadResource )
	{'''

replacement = '''XIAHGE_API void XiahGameEngine::CEffect::RealizeEffectResouce(_EFFECT* pEffect)
{
    if (!pEffect) return;
    __try {
	if( !pEffect->m_bLoadResource )
	{'''

target2 = '''		// register in effect mesh pool
			m_pEffectResPool->RegisterHCHMeshFromFile( strMeshPath, fp );

		}// for(pEffect->m_MeshList)

		pEffect->m_bLoadResource = true;
	}// if
}'''

replacement2 = '''		// register in effect mesh pool
			m_pEffectResPool->RegisterHCHMeshFromFile( strMeshPath, fp );

		}// for(pEffect->m_MeshList)

		pEffect->m_bLoadResource = true;
	}// if
    } __except(1) {
        return;
    }
}'''

if target in content and target2 in content:
    content = content.replace(target, replacement)
    content = content.replace(target2, replacement2)
    with open(file_path, 'w', encoding='euc-kr') as f:
        f.write(content)
    print('Patch applied successfully.')
else:
    print('Target not found.')
