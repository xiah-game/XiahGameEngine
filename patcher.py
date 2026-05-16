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

# Waterfall.cpp
waterfall_reps = [
    (
        b'for(wit=m_ListWaterBottom.begin(); wit!=m_ListWaterBottom.end(); wit++)\r\n\t{\r\n\t\tCParticleInfo* pParticle = *wit;\r\n\r\n\t\tif(pParticle)\r\n\t\t{\r\n\t\t\tpParticle->fScale += 0.07f;\r\n\t\t\tpParticle->fAlpha -= 0.06f;\r\n\r\n\t\t\tif( dwCurTick - pParticle->m_dwCreateTime > pParticle->fTTL )\r\n\t\t\t{\r\n\t\t\t\twit = m_ListWaterBottom.erase(wit);\r\n\t\t\t\tdelete pParticle;\r\n\t\t\t}// if\r\n\t\t}// if(pParticle)\r\n\r\n\t}// for( m_ListWaterBottom )',
        b'for(wit=m_ListWaterBottom.begin(); wit!=m_ListWaterBottom.end(); )\r\n\t{\r\n\t\tCParticleInfo* pParticle = *wit;\r\n\r\n\t\tif(pParticle)\r\n\t\t{\r\n\t\t\tpParticle->fScale += 0.07f;\r\n\t\t\tpParticle->fAlpha -= 0.06f;\r\n\r\n\t\t\tif( dwCurTick - pParticle->m_dwCreateTime > pParticle->fTTL )\r\n\t\t\t{\r\n\t\t\t\twit = m_ListWaterBottom.erase(wit);\r\n\t\t\t\tdelete pParticle;\r\n\t\t\t}\r\n\t\t\telse { wit++; }\r\n\t\t}\r\n\t\telse { wit++; }\r\n\r\n\t}// for( m_ListWaterBottom )'
    )
]

effectres_reps = [
    # 1. it == NULL removal
    (
        b'for(it = elist.begin(); it != elist.end(); it++)\r\n\t{\r\n\t\tEFFECTDATA &value = *it;\r\n\t\tif(it == NULL)\r\n\t\t{\r\n\t\t\tDBG_LogFile( _T("ScaleEffectDataTime fail"));\r\n\t\t}\r\n\t\tvalue.fTime *= scale;\r\n\t}',
        b'for(it = elist.begin(); it != elist.end(); it++)\r\n\t{\r\n\t\tEFFECTDATA &value = *it;\r\n\t\tvalue.fTime *= scale;\r\n\t}'
    ),
    # 2. empty texture
    (
        b'if( filename != _T("") )\r\n\t\t{\r\n\t\t\tMyString strFile = _T("");\r\n\t\t\tit = m_TextureMap.find(strFile.data());\r\n\r\n\t\t\tpEffectTexture = it->second;\r\n\t\t\tpEffectTexture->m_nRefCount++;\r\n\t\t}',
        b'if( filename != _T("") )\r\n\t\t{\r\n\t\t\tMyString strFile = _T("");\r\n\t\t\tit = m_TextureMap.find(strFile.data());\r\n\r\n\t\t\tif(it != m_TextureMap.end()) {\r\n\t\t\t\tpEffectTexture = it->second;\r\n\t\t\t\tpEffectTexture->m_nRefCount++;\r\n\t\t\t} else {\r\n\t\t\t\tpEffectTexture = new _EFFECTTEXTURE;\r\n\t\t\t\tpEffectTexture->m_nRefCount = 1;\r\n\t\t\t\tm_TextureMap.insert(EFFECTTEXTUREMAP::value_type(strFile.data(), pEffectTexture));\r\n\t\t\t}\r\n\t\t}'
    ),
    # 3. DeleteEffectRenerList
    (
        b'for(erit=DeleteEffectRenerList.begin(); erit!=DeleteEffectRenerList.end(); erit++)\r\n\t{\r\n\t\t_EFFECTRENDER* pEffectRender = *erit;\r\n\r\n\t\tDeleteEffectRender( pEffectRender );\r\n\t\tm_EffectRenderList.remove( pEffectRender );\r\n\r\n\t\tm_CurEffectRenderList.remove( pEffectRender );\r\n\r\n\t\terit = DeleteEffectRenerList.erase( erit );\r\n\t}// for( DeleteEffectRenerList )',
        b'for(erit=DeleteEffectRenerList.begin(); erit!=DeleteEffectRenerList.end(); )\r\n\t{\r\n\t\t_EFFECTRENDER* pEffectRender = *erit;\r\n\r\n\t\tDeleteEffectRender( pEffectRender );\r\n\t\tm_EffectRenderList.remove( pEffectRender );\r\n\r\n\t\tm_CurEffectRenderList.remove( pEffectRender );\r\n\r\n\t\terit = DeleteEffectRenerList.erase( erit );\r\n\t}// for( DeleteEffectRenerList )'
    ),
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
    # 6. m_ElementList header
    (
        b'for(erit=pParticleRender->m_ElementList.begin(); erit!=pParticleRender->m_ElementList.end(); erit++)',
        b'for(erit=pParticleRender->m_ElementList.begin(); erit!=pParticleRender->m_ElementList.end(); )'
    ),
    # 7. m_ElementList life time continue
    (
        b'if( pElementRender->m_ElapsedTime > pElementRender->m_pElement->m_LifeTime )\r\n\t\t\t\t{\r\n\t\t\t\t\tpElementRender->m_bPlay = false;\r\n\t\t\t\t\tcontinue;\r\n\t\t\t\t}',
        b'if( pElementRender->m_ElapsedTime > pElementRender->m_pElement->m_LifeTime )\r\n\t\t\t\t{\r\n\t\t\t\t\tpElementRender->m_bPlay = false;\r\n\t\t\t\t\terit++;\r\n\t\t\t\t\tcontinue;\r\n\t\t\t\t}'
    ),
    # 8. m_ElementList loop end
    (
        b'}// if( pElementRender->m_pElement->m_Type == PET_Mesh )\r\n\r\n\t\t}// for(pParticleRender->m_ElementList[i])',
        b'}// if( pElementRender->m_pElement->m_Type == PET_Mesh )\r\n\t\t\terit++;\r\n\t\t}// for(pParticleRender->m_ElementList[i])'
    )
]

modify_file(r'D:\xiahold\XiahGameEngine\Waterfall.cpp', waterfall_reps)
modify_file(r'D:\xiahold\XiahGameEngine\EffectRes.cpp', effectres_reps)
