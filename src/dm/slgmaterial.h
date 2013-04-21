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

//forward declare.
class MD5;
class aiMaterial;

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



/** @brief 本类保存了额外的材质信息。可以看作对slg material基类的信息添加。
**/
struct  ExtraMaterial
{
protected:
    friend  class DocIO;
    std::string     m_slgname;
public:
    std::string     m_name;
    ExtraMaterial(){
    }
    ExtraMaterial(const std::string &slgname) : m_slgname(slgname)
    {
    }
    ExtraMaterial(const ExtraMaterial& ref){
        AssignFrom(ref);
    }
    ~ExtraMaterial(){
    }
    ExtraMaterial&  operator=(const ExtraMaterial& ref){
        AssignFrom(ref);
        return *this;
    }
    inline  const std::string&  name(void)const{
        return m_name;
    }
    inline  void  name(const std::string& n){
        m_name = n;
    }
    inline  const std::string&  slgname(void)const{
        return m_slgname;
    }
    inline  std::string getNameForSlg(const boost::uuids::uuid &id)const
    {
        if(this->m_slgname.length())
            return this->m_slgname;
        return ObjectNode::idto_string(id);
    }
private:
    void    AssignFrom(const ExtraMaterial& ref)
    {
        this->m_slgname = ref.m_slgname;
        this->m_name = ref.m_name;
    }
};

class ExtraMaterialManager
{
private:
    friend class DocPrivate;
    ExtraMaterialManager(){}
    ~ExtraMaterialManager(){}

    typedef  boost::unordered_map<boost::uuids::uuid, ExtraMaterial>        type_id2matinfo_map;
    /** @brief 保存了从material id 到material_map.
    **/
    type_id2matinfo_map                    m_id2matinfo_map;
public:
    ///@brief 清空全部数据，用于场景重置(例如加载)。
    void    clear(){
        m_id2matinfo_map.clear();
    }
    ///@brief 查询及获取方法。返回对应的materialInfo记录，如果没有，则返回空。
    inline ExtraMaterial*  query(const boost::uuids::uuid &id){
        type_id2matinfo_map::iterator it = m_id2matinfo_map.find(id);
        if(it != m_id2matinfo_map.end())
        {
            return &(it->second);
        }
        return NULL;
    }
    inline  void       eraseMaterialInfo(const boost::uuids::uuid &id)
    {
        ///@fixme 需要继续清理掉引用的ExtraTexture信息。
        m_id2matinfo_map.erase(id);
    }
    type_id2matinfo_map::iterator  queryFromSlgname(const std::string &slgname){
        type_id2matinfo_map::iterator it = m_id2matinfo_map.begin();
        while(it != m_id2matinfo_map.end())
        {
            if(it->second.slgname() == slgname)
            {
                return it;
            }
            it++;
        }
        return it;
    }

    /** @brief getXXX函数如果没有找到对应记录，会自动添加一个materialInfo记录。
    **/
    inline ExtraMaterial&  get(const boost::uuids::uuid &id)
    {
        return m_id2matinfo_map[id];
    }

    ExtraMaterial&  getFromSlgname(const std::string &slgname)
    {
        type_id2matinfo_map::iterator it = queryFromSlgname(slgname);
        if(it != m_id2matinfo_map.end())
        {
            return it->second;
        }
        ExtraMaterial   em(slgname);
        em.name(slgname);
        boost::uuids::uuid id = boost::uuids::random_generator()();
        m_id2matinfo_map[id] = em;
        return m_id2matinfo_map[id];
    }

    void    write(MaterialWriteContext &ctx,const std::vector<boost::uuids::uuid> &outset);
    boost::uuids::uuid  createAssimpMaterial(aiMaterial *paiMat,SlgUtil::Editor &editor);
    static  boost::uuids::uuid  createGrayMaterial(void);
    boost::uuids::uuid  createGrayMaterial(const std::string &name);
    ///@brief 改进slg缺陷，递归检查一个材质是否是光源。
    static  bool    materialIsLight(const slg::Material *pmat);
private:
    /** @brief 实际写入一个材质到ostream.
     * @param ppmd5 父md5标识，可以传入空。
     * @return 返回本Material最后写入的id.
    */
    std::string    WriteMaterialImpl(MaterialWriteContext &ctx,
                                        const slg::Material *pMat,
                                        const boost::uuids::uuid *pId,
                                        const std::string &name,
                                        const std::string &slgname,
                                        MD5 *ppmd5);
};

}

#endif  //GME_DM_SLGMATERIAL_H
