import os

def modify_file(filepath, replacements):
    with open(filepath, 'rb') as f:
        content = f.read()
    
    original = content
    for old, new in replacements:
        if old not in content:
            print(f"Warning: Could not find snippet starting with {old[:30]}...")
        else:
            content = content.replace(old, new)
            print(f"Replaced snippet starting with {old[:30]}...")
            
    if content != original:
        with open(filepath, 'wb') as f:
            f.write(content)
        print(f"Successfully updated {filepath}")
    else:
        print(f"No changes needed for {filepath}")

effectres_reps2 = [
    # 4. m_MeshMap
    (
        b'for(mit=m_MeshMap.begin(); mit!=m_MeshMap.end(); mit++)\r\n\t\t\t{\r\n\t\t\t\t_EFFECTMESH* pEMesh = mit->second;\r\n\t\t\t\tCString strMesh = mit->first;\r\n\r\n\t\t\t\tif( strMesh == strFilename  ) continue; // same file\r\n\t\t\t\tif( pEMesh->m_nRefCount > 1 ) continue;\r\n\r\n\t\t\t\t// delete this mesh\r\n\t\t\t\tm_nCurMeshAllocated -= pEMesh->m_nAllocSize;\r\n\r\n\t\t\t\tmit = m_MeshMap.erase(mit);\r\n\t\t\t\tdelete pEMesh;\r\n\t\t\t}',
        b'for(mit=m_MeshMap.begin(); mit!=m_MeshMap.end(); )\r\n\t\t\t{\r\n\t\t\t\t_EFFECTMESH* pEMesh = mit->second;\r\n\t\t\t\tCString strMesh = mit->first;\r\n\r\n\t\t\t\tif( strMesh == strFilename  ) { mit++; continue; } // same file\r\n\t\t\t\tif( pEMesh->m_nRefCount > 1 ) { mit++; continue; }\r\n\r\n\t\t\t\t// delete this mesh\r\n\t\t\t\tm_nCurMeshAllocated -= pEMesh->m_nAllocSize;\r\n\r\n\t\t\t\tmit = m_MeshMap.erase(mit);\r\n\t\t\t\tdelete pEMesh;\r\n\t\t\t}'
    ),
    # 5. m_TextureMap
    (
        b'for(tit=m_TextureMap.begin(); tit!=m_TextureMap.end(); tit++)\r\n\t\t\t{\r\n\t\t\t\t_EFFECTTEXTURE* pETexture = tit->second;\r\n\t\t\t\tCString strTexture = tit->first;\r\n\r\n\t\t\t\tif( strTexture == strFile ) continue;\t\t\t// same file so this texture\r\n\t\t\t\tif( pETexture->m_nRefCount > 1 ) continue;\t\t// now using\r\n\r\n\t\t\t\t// delete this texture\r\n\t\t\t\tm_nCurTextureAllocated -= pETexture->m_nAllocSize;\r\n\r\n\t\t\t\ttit = m_TextureMap.erase(tit);\r\n\t\t\t\tdelete pETexture;\r\n\t\t\t}',
        b'for(tit=m_TextureMap.begin(); tit!=m_TextureMap.end(); )\r\n\t\t\t{\r\n\t\t\t\t_EFFECTTEXTURE* pETexture = tit->second;\r\n\t\t\t\tCString strTexture = tit->first;\r\n\r\n\t\t\t\tif( strTexture == strFile ) { tit++; continue; }\t\t\t// same file so this texture\r\n\t\t\t\tif( pETexture->m_nRefCount > 1 ) { tit++; continue; }\t\t// now using\r\n\r\n\t\t\t\t// delete this texture\r\n\t\t\t\tm_nCurTextureAllocated -= pETexture->m_nAllocSize;\r\n\r\n\t\t\t\ttit = m_TextureMap.erase(tit);\r\n\t\t\t\tdelete pETexture;\r\n\t\t\t}'
    ),
    # 7. m_ElementList life time continue
    (
        b'if( pElementRender->m_ElapsedTime > pElementRender->m_pElement->m_LifeTime )\r\n\t\t\t\t\t\t{\r\n\t\t\t\t\t\t\tpElementRender->m_bPlay = false;\r\n\t\t\t\t\t\t\tcontinue;\r\n\t\t\t\t\t\t}',
        b'if( pElementRender->m_ElapsedTime > pElementRender->m_pElement->m_LifeTime )\r\n\t\t\t\t\t\t{\r\n\t\t\t\t\t\t\tpElementRender->m_bPlay = false;\r\n\t\t\t\t\t\t\terit++;\r\n\t\t\t\t\t\t\tcontinue;\r\n\t\t\t\t\t\t}'
    ),
    # 8. m_ElementList loop end
    (
        b'}// if( pElementRender->m_pElement->m_Type == PET_Mesh )\r\n\t\t\t\t\t\r\n\t\t\t\t}// for(pParticleRender->m_ElementList[i])',
        b'}// if( pElementRender->m_pElement->m_Type == PET_Mesh )\r\n\t\t\t\t\terit++;\r\n\t\t\t\t}// for(pParticleRender->m_ElementList[i])'
    )
]

modify_file(r'D:\xiahold\XiahGameEngine\EffectRes.cpp', effectres_reps2)
