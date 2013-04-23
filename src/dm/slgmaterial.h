//////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2013 by sanpolo CO.LTD                                    //
//                                                                          //
//  This file is part of GMEditor                                           //
//                                                                          //
//  GMEditor is free software; you can redistribute it and/or modify it     //
//  under the terms of the LGPL License.                                    //
//                                                                          //
//  GMEditor is distributed in the hope that it will be useful,but WITHOUT  //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY      //
//  or FITNESS FOR A PARTICULAR PURPOSE.                                    //
//                                                                          //
//  You should have received a copy of the LGPL License along with this     //
//  program.  If not, see <http://www.render001.com/gmeditor/licenses>.     //
//                                                                          //
//  GMEditor website: http://www.render001.com/gmeditor                     //
//////////////////////////////////////////////////////////////////////////////

#ifndef  GME_DM_SLGMATERIAL_H
#define  GME_DM_SLGMATERIAL_H

#include "utils/singleton.h"
#include "slg/rendersession.h"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include "slgtexture.h"
#include "dm/objectnode.h"
#include "slgutils.h"
#include "dm/doc.h"

//forward declare.
class MD5;
struct aiMaterial;

namespace gme{

class  SlgMaterial2Name
{
private:
    ///@brief 用于从material出发反向查找name的变量。
    ///@todo: 是否有必要在ExtraMaterialManager中建立长期缓冲？
    std::vector<u_int>      m_matIdx2NameIdx;
    std::vector< std::string >  m_materialNameArray;
public:
    SlgMaterial2Name(void);
    SlgMaterial2Name(const SlgMaterial2Name &ref)
    {
        this->m_materialNameArray = ref.m_materialNameArray;
        this->m_matIdx2NameIdx = ref.m_matIdx2NameIdx;
    }
    const std::string&      getMaterialName(const slg::Material* pmat)const;
};

struct  MaterialWriteContext{
protected:
	inline	void outIndent(void)
	{
		for(int indent = 0; indent < this->m_indent; indent++)
			stream() << "  ";
	}
    TextureWriteContext m_texCtx;
    const SlgMaterial2Name        m_mat2name;
    void    buildSlgMatidx2Nameidx();
    friend  class ExtraMaterialManager;
    /** @brief 保存了材质内容码到材质id的映射。这允许我们正确加入use属性。
    **/
    typedef   boost::unordered_map<std::string, std::string>    type_ctx2id;
    type_ctx2id             m_ctx2id;

    const bool                      m_bSaveRes;
    const boost::filesystem::path   &m_dest_path;
	int				m_indent;
    std::ostream	*m_pstream;

	inline std::ostream&	stream(){
		return *m_pstream;
	}
public:
    MaterialWriteContext(bool bExportRes,const boost::filesystem::path &p,std::ostream &o,int indent = 1)
        : m_texCtx(bExportRes,p,o,indent),
          m_bSaveRes(bExportRes),
          m_dest_path(p),
          m_indent(indent)
    {
        m_pstream = &o;
    }
    ~MaterialWriteContext(){}
};


class ExtraMaterialManager
{
private:
    friend class DocPrivate;
    ExtraMaterialManager(){}
    ~ExtraMaterialManager(){}

    ///@brief 保存了从材质id到材质名称的map.材质id就是slgname.
    typedef  boost::unordered_map<std::string, std::string>        type_id2name_map;
    /** @brief 保存了从material id 到material name.
    **/
    type_id2name_map                    m_id2name;

    typedef  boost::unordered_map<const slg::Material*,std::string>     type_mat2id;
    type_mat2id                         m_mat2id;
public:
    ///@brief 清空全部数据，用于场景重置(例如加载)。
    void    clear(){
        m_id2name.clear();
        m_mat2id.clear();
    }

    ///@brief 查询及获取方法。返回对应的materialName。
    inline std::string&  get(const std::string &id){
        return m_id2name[id];
    }

    inline  void       eraseMaterialInfo(const std::string &id,const slg::Material *pMat)
    {
        ///@fixme 需要继续清理掉引用的ExtraTexture信息。
        m_id2name.erase(id);
        if(pMat)
        {
            m_mat2id.erase(pMat);
        }
    }

    void    appendMat2IdFromSlg(void);
    inline  const std::string&  getMaterialId(const slg::Material* pMat)
    {
        return m_mat2id[pMat];
    }

    /** @brief
    **/
    static slg::Material* getSlgMaterial(const std::string &id);

    /** @brief 将指定材质的内容导出到node中。
    **/
//    inline bool   dump(const std::string &id,type_node &node)
//    {
//        slg::Material* pmat = getSlgMaterial(id);
//        if(pmat)
//        {
//            dump(pmat,&id,node);
//            return true;
//        }
//        return false;
//    }
    ///@todo 从type_node对象中恢复材质。
    //void    exportResource(type_node &node,boost::);

    ///@fixme : 使用rapidxml接口来导入导出材质。
    //void    write(MaterialWriteContext &ctx,const std::vector<boost::uuids::uuid> &outset);
    //std::string  createAssimpMaterial(aiMaterial *paiMat,SlgUtil::Editor &editor);
    static  void createGrayMaterial(const std::string& id);
    //boost::uuids::uuid  createGrayMaterial(const std::string &name);
    ///@brief 改进slg缺陷，递归检查一个材质是否是光源。
    static  bool    materialIsLight(const slg::Material *pmat);

    type_xml_node*   dump(type_xml_node &parent,const slg::Material* pMat,dumpContext &ctx);
private:
//    void    dump(slg::Material *pMat,
//                    const boost::uuids::uuid *pId,
//                    type_node &node);

    /** @brief 实际写入一个材质到ostream.
     * @param ppmd5 父md5标识，可以传入空。
     * @return 返回本Material最后写入的id.
    */
    /*std::string    WriteMaterialImpl(MaterialWriteContext &ctx,
                                        const slg::Material *pMat,
                                        const boost::uuids::uuid *pId,
                                        const std::string &name,
                                        MD5 *ppmd5);*/
};

}

#endif  //GME_DM_SLGMATERIAL_H
