/*
    The MIT License (MIT)

    Copyright (c) 2016 Benoit AUTHEMAN

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

//-----------------------------------------------------------------------------
// This file is a part of the GTpo software library.
//
// \file	gtpoGenNode.h
// \author	benoit@qanava.org
// \date	2016 01 22
//-----------------------------------------------------------------------------

#ifndef gtpoGenNode_h
#define gtpoGenNode_h

// STD headers
#include <list>
#include <unordered_set>
#include <memory>           // std::shared_ptr std::weak_ptr and std::make_shared
#include <functional>       // std::hash
#include <cassert>
#include <iterator>         // std::back_inserter

// GTpo headers
#include "./gtpoUtils.h"
#include "./gtpoBehaviour.h"
#include "./gtpoGraphConfig.h"

namespace gtpo { // ::gtpo

template <class Config>
class GenGraph;

template <class Config>
class GenGroup;

/*! \brief Base class for modelling nodes with an in/out edges list in a gtpo::GenGraph graph.
 *
 * \nosubgrouping
 */
template <class Config = DefaultConfig>
class GenNode : public Config::NodeBase,
                public gtpo::Behaviourable< gtpo::NodeBehaviour< Config >,
                                            typename Config::NodeBehaviours >,
                public std::enable_shared_from_this<typename Config::FinalNode>
{
    friend GenGraph<Config>;   // GenGraph need access to setGraph()

    /*! \name Node Management *///---------------------------------------------
    //@{
public:
    using Graph         = GenGraph<Config>;
    using WeakNode      = std::weak_ptr< typename Config::FinalNode >;
    using SharedNode    = std::shared_ptr< typename Config::FinalNode >;
    using WeakNodes     = typename Config::template NodeContainer< WeakNode >;

    //! User friendly shortcut type to this concrete node Behaviourable base type.
    using BehaviourableBase = gtpo::Behaviourable< gtpo::NodeBehaviour< Config >,
                                                   typename Config::NodeBehaviours >;

    GenNode() noexcept : Config::NodeBase{} { }
    ~GenNode() {
        _inEdges.clear(); _outEdges.clear();
        _inNodes.clear(); _outNodes.clear();
        if ( _graph != nullptr ) {
            std::cerr << "gtpo::GenNode<>::~GenNode(): Warning: Node has been destroyed before beeing removed from the graph." << std::endl;
        }
        _graph = nullptr;
    }
    GenNode(const GenNode& node ) = delete;
    GenNode& operator=( GenNode const& ) = delete;
protected:
    inline  Graph*          getGraph() noexcept { return _graph; }
    inline  const Graph*    getGraph() const noexcept { return _graph; }
private:
    inline void             setGraph( Graph* graph ) noexcept { _graph = graph; }
public: // FIXME
    Graph*                  _graph{ nullptr };
    //@}
    //-------------------------------------------------------------------------

    /*! \name Node Edges Management *///---------------------------------------
    //@{
public:
    using WeakEdge      = std::weak_ptr< typename Config::FinalEdge >;
    using WeakEdges     = typename Config::template EdgeContainer< WeakEdge >;
    using SharedEdge    = std::shared_ptr< typename Config::FinalEdge >;

    /*! \brief Insert edge \c outEdge as an out edge for this node.
     *
     * \note if \c outEdge source node is different from this node, it is set to this node.
     */
    auto    addOutEdge( WeakEdge sharedOutEdge ) noexcept( false ) -> void;
    /*! \brief Insert edge \c inEdge as an in edge for \c node.
     *
     * \note if \c inEdge destination node is different from \c node, it is automatically set to \c node.
     */
    auto    addInEdge( WeakEdge sharedInEdge ) noexcept( false ) -> void;
    /*! \brief Remove edge \c outEdge from this node out edges.
     *
     * \throw gtpo::bad_topology_error
     */
    auto    removeOutEdge( const WeakEdge outEdge ) noexcept( false ) -> void;
    /*! \brief Remove edge \c inEdge from this node in edges.
     *
     * \throw gtpo::bad_topology_error
     */
    auto    removeInEdge( const WeakEdge inEdge ) noexcept( false ) -> void;

    inline auto     getInEdges() const noexcept -> const WeakEdges& { return _inEdges; }
    inline auto     getOutEdges() const noexcept -> const WeakEdges& { return _outEdges; }

    inline auto     getInNodes() const noexcept -> const WeakNodes& { return _inNodes; }
    inline auto     getOutNodes() const noexcept -> const WeakNodes& { return _outNodes; }

    inline auto     getInDegree() const noexcept -> unsigned int { return static_cast<int>( _inEdges.size() ); }
    inline auto     getOutDegree() const noexcept -> unsigned int { return static_cast<int>( _outEdges.size() ); }
private:
    WeakEdges       _inEdges;
    WeakEdges       _outEdges;
    WeakNodes       _inNodes;
    WeakNodes       _outNodes;
    //@}
    //-------------------------------------------------------------------------

    /*! \name Node Edges Management *///---------------------------------------
    //@{
public:
    using WeakGroup = std::weak_ptr< typename Config::FinalGroup >;
    inline auto setGroup( WeakGroup& group ) noexcept -> void { _group = group; }
    inline auto getGroup( ) noexcept -> WeakGroup& { return _group; }
    inline auto getGroup( ) const noexcept -> const WeakGroup& { return _group; }
private:
    WeakGroup   _group;
    //@}
    //-------------------------------------------------------------------------

    /*! \name Node Behaviours Notifications *///-------------------------------
    //@{
    inline auto    notifyInNodeInserted( WeakNode& outNode ) noexcept -> void;
    inline auto    notifyInNodeAboutToBeRemoved( WeakNode& outNode ) noexcept -> void;
    inline auto    notifyInNodeRemoved( ) noexcept -> void;
    inline auto    notifyOutNodeInserted( WeakNode& outNode ) noexcept -> void;
    inline auto    notifyOutNodeRemoved( WeakNode& outNode ) noexcept -> void;
    inline auto    notifyOutNodeRemoved() noexcept -> void;
    //@}
    //-------------------------------------------------------------------------
};

} // ::gtpo

#include "./gtpoGenNode.hpp"

#endif // gtpoGenNode_h

