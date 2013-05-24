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

#ifndef  GME_DM_MESHNODE_H
#define  GME_DM_MESHNODE_H

#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <Eigen/Core>
#include <fstream>
#include "dm/xmlutil.h"

namespace luxrays{
    class BBox;
}

namespace gme{

class ObjectNode;

/**
 * @brief 可以把ObjectNodePath当作标准STL中的vector<ObjectNode*>来用，只不过增加了几个便利方法。
 */
class ObjectNodePath : public std::vector<ObjectNode*>
{
private:
    typedef std::vector<ObjectNode*>	inherited;
    size_type				m_depth;
public:
    /// for compatible with stl.
    typedef inherited::value_type					value_type;
    typedef inherited::pointer						pointer;
    typedef inherited::const_pointer				const_pointer;
    typedef inherited::reference					reference;
    typedef inherited::const_reference				const_reference;
    typedef inherited::iterator						iterator;
    typedef inherited::const_iterator				const_iterator;
    typedef inherited::const_reverse_iterator		const_reverse_iterator;
    typedef inherited::reverse_iterator				reverse_iterator;
    typedef inherited::size_type					size_type;
    typedef inherited::difference_type				difference_type;
    typedef inherited::allocator_type				allocator_type;

    ObjectNodePath(void) : m_depth(0){}
    ObjectNodePath(int reservPathLength) :inherited(reservPathLength),m_depth(0){}
    ObjectNodePath(ObjectNode *head) : m_depth(0){
        append(head);
    }

    ObjectNodePath(const ObjectNodePath & rhs) : inherited(rhs) , m_depth(rhs.m_depth) {}
    ObjectNodePath & operator=(const ObjectNodePath & rhs){
        *dynamic_cast<inherited*>(this) = rhs;
        m_depth = rhs.m_depth;
        return *this;
    }


    void setHead(ObjectNode * head){
        if(inherited::size()){
            inherited::at(0) = head;
        }else{
            inherited::push_back(head);
            m_depth++;
        }
    }

    inline size_type	size() const{
        return m_depth;
    }

    void append(ObjectNode * node){
        if(inherited::size() > m_depth)
        {
            inherited::at(m_depth++) = node;
        }else{
            inherited::push_back(node);
            m_depth++;
        }
    }

    void append(const ObjectNodePath &frompath){
        if( frompath.size() + size() > inherited::size() )
        {
            inherited::resize(frompath.size() + size());
        }
        std::copy(frompath.begin(),frompath.begin() + frompath.size(),inherited::begin() + size());
    }

    inline void push(ObjectNode * node){
        append(node);
    }

    void resize(size_type __new_size, value_type __x = value_type())
    {
        if(__new_size > inherited::size())
        {
            inherited::resize(__new_size,__x);
            m_depth = __new_size;
        }else{
            m_depth = __new_size;
        }
    }

    void pop(void){
        m_depth--;
    }

    iterator end()
    { return inherited::begin() + size();}

    const_iterator	end() const
    { return inherited::begin() + size(); }

    ObjectNode * getHead(void) const
    {
        return inherited::size() ? inherited::at(0) : NULL;
    }

    ObjectNode * getTail(void) const
    {
        return inherited::size() ? inherited::at(m_depth - 1) : NULL;
    }

    ObjectNode * getNode(size_type index) const{
        return (index < m_depth) ? inherited::at(index) : NULL;
    }
    ObjectNode * getNodeFromTail(size_type index) const{
        return (index < m_depth) ? inherited::at(m_depth - index - 1) : NULL;
    }

};

/** @brief 保存了对象的额外信息。
**/
class ObjectNode
{
private:
    friend class DocIO;
    friend class ExtraObjectManager;
protected:
    //以字符串形式保存了模型的id.这个id直接作为slgname使用。
    std::string             m_id;
    //扩展保存了name。这允许我们自定义重复的名称作为模型名称。
    std::string             m_name;
    /** @brief 模型文件。m_filepath保存了本mesh的原始文件。
      * @details 这个原始文件可能保存了若干模型.将会被展开为子模型节点。
    **/
    std::string             m_filepath;
    /** @brief 这里以string类型保存material id.matID就是slgname.
    **/
    std::string             m_matid;
    std::vector<ObjectNode> m_children;
    bool                    m_useplynormals;
    /** @brief 指示m_filepath中所包含的模型是否是一个
    **/
    bool                    m_bGroupfile;
    /** @brief m_transformation is difference from slg. this is a hierarchy.
    **/
    Eigen::Matrix4f         m_transformation;
    inline void    assignFrom(const ObjectNode& ref)
    {
        m_id = ref.m_id;
        m_name = ref.m_name;
        m_filepath = ref.m_filepath;
        m_matid = ref.m_matid;
        m_children = ref.m_children;
        m_useplynormals = ref.m_useplynormals;
        m_transformation = ref.m_transformation;
    }
    ///@brief notify some child removed.
    void    onChildRemoved(const std::string &childid);
public:
    void    unionBBox(luxrays::BBox  *pbox);
    /** @brief 将自身的矩阵应用到matrix上以获取世界坐标系到对象坐标系的变幻矩阵。
    **/
    void    applyMatrix(Eigen::Matrix4f &matrix);
    /** @brief 在调用之前，对象矩阵已经设置完毕，只需要绘制自身即可。
    **/
    void    drawSelf(void);
    /** @brief 在父矩阵matrix环境下，绘制自己及孩子。需要设置自身的矩阵。
    **/
    void    draw(const Eigen::Matrix4f &matrix);
    /** @brief 将当前对象的内容dump到给定的xml node.
      * @details 如果使用相对路径或者拷贝资源或者导出ctm,都是相对于当前路径展开的计算，因此调用之前需要设置当前路径为资源目标路径。
      * @return self node.
    **/
    type_xml_node*   dump(type_xml_node &parent,dumpContext &ctx);

    /** @brief 搜索值为id的节点对象。
     *  @param parent 如果给出此参数，将把父对象写入这里。
    **/
    ObjectNode*     findObject(const std::string &id,ObjectNodePath *pPath);
    inline void addChild(const ObjectNode&  child)
    {
        m_children.push_back(child);
    }
    inline  bool    removeChild(const std::string &id)
    {
        std::vector<ObjectNode>::iterator it = m_children.begin();
        while(it != m_children.end())
        {
            if(it->m_id == id)
            {
                //must remove before earse,otherwise id will been destroy!
                onChildRemoved(id);
                m_children.erase(it);
                return true;
            }else if(it->removeChild(id))
            {
                return true;
            }
            it++;
        }
        return false;
    }
    inline void clear(){
        m_children.clear();
    }
    typedef std::vector<ObjectNode> type_child_container;
    inline type_child_container::size_type size(void)const
    {
        return m_children.size();
    }
    inline type_child_container::iterator  begin(){
        return m_children.begin();
    }
    inline type_child_container::iterator  end(){
        return m_children.end();
    }
    inline type_child_container::const_iterator  begin()const{
        return m_children.begin();
    }
    inline type_child_container::const_iterator  end()const{
        return m_children.end();
    }
    inline  const std::string&     id(void)const{
        return m_id;
    }
    inline  const std::string&     matid(void)const{
        return m_matid;
    }
	inline	const std::string&		name(void)const{
        if(m_name.length() == 0)
            return m_id;
		return m_name;
	}
    ///@brief 指示本节点只是一个组节点，没有实际的模型对应。
    inline  bool    isPureGroup(void)const
    {
        return (m_matid.length() == 0 && m_filepath.length() == 0);
    }
    int     getChildCount(void)const
    {
        int ret = 1;
        type_child_container::const_iterator  it = begin();
        while(it != end())
        {
            ret += it->getChildCount();
            it++;
        }
        return ret;
    }
	inline  void    name(const std::string &n){
	    m_name = n;
	}
	inline  const std::string&  filepath(void){
	    return m_filepath;
    }
	inline  bool    useplynormals()const{
	    return m_useplynormals;
    }
    inline  void    useplynormals(bool buse)
    {
        m_useplynormals = buse;
    }
    ObjectNode()
    {
        m_useplynormals = false;
        //创建者需要自行维护nil.
//        m_id = boost::uuids::nil_generator()();
//        m_matid = boost::uuids::nil_generator()();
    }
    ObjectNode(const ObjectNode& ref){
        assignFrom(ref);
    }
    ~ObjectNode()
    {
        clear();
    }
    ObjectNode& operator=(const ObjectNode& ref){
        assignFrom(ref);
        return *this;
    }
};

}

#endif //GME_DM_MESHNODE_H
