/*
Copyright (C) 2001-2006, William Joseph.
All Rights Reserved.

This file is part of GtkRadiant.

GtkRadiant is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

GtkRadiant is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GtkRadiant; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#if !defined(INCLUDED_MODEL_H)
#define INCLUDED_MODEL_H

#include "entitylib.h"
#include "traverselib.h"
#include "generic/callback.h"
#include "stream/stringstream.h"
#include "os/path.h"
#include "moduleobserver.h"

class Model : public ModuleObserver
{
  ResourceReference m_resource;
  scene::Traversable& m_traverse;
  scene::INodePtr m_node;

public:
  Model(scene::Traversable& traversable)
    : m_resource(""), m_traverse(traversable)
  {
    m_resource.attach(*this);
  }
  ~Model()
  {
    m_resource.detach(*this);
  }

  void realise()
  {
    m_resource.get()->load();
    m_node = m_resource.get()->getNode();
    if(m_node != 0)
    {
      m_traverse.insert(m_node);
    }
  }
  void unrealise()
  {
    if(m_node != 0)
    {
      m_traverse.erase(m_node);
    }
  }
  
	// Update the model to the provided keyvalue
	void modelChanged(std::string val);
  
  typedef MemberCaller1<Model, std::string, &Model::modelChanged> ModelChangedCaller;

  const char* getName() const
  {
    return m_resource.getName();
  }
  scene::INodePtr getNode() const
  {
    return m_node;
  }
};

class SingletonModel
{
  TraversableNode m_traverse;
  Model m_model;
public:
  SingletonModel()
    : m_model(m_traverse)
  {
  }

  void attach(scene::Traversable::Observer* observer)
  {
    m_traverse.attach(observer);
  }
  void detach(scene::Traversable::Observer* observer)
  {
    m_traverse.detach(observer);
  }

  scene::Traversable& getTraversable()
  {
    return m_traverse;
  }
  const scene::Traversable& getTraversable() const
  {
    return m_traverse;
  }

  void modelChanged(const std::string& value)
  {
    m_model.modelChanged(value);
  }
  typedef MemberCaller1<SingletonModel, const std::string&, &SingletonModel::modelChanged> ModelChangedCaller;

  scene::INodePtr getNode() const
  {
    return m_model.getNode();
  }
};

#endif
