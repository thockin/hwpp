/* Copyright (c) Tim Hockin, 2008 */
#ifndef PP_PP_PLATFORM_H__
#define PP_PP_PLATFORM_H__

#include "pp.h"
#include "fake_language.h"
#include "device_init.h"
#include "drivers.h"

class pp_platform
{
    public:
	pp_platform()
	{
		// platform is the top level, cur_scope must not exist
		DASSERT_MSG(!current_context.is_valid(),
				"current_context is already valid");
		DASSERT_MSG(context_stack.empty(),
				"context_stack must be empty");

		OPEN_SCOPE("");
		platform_global_init(current_context.scope());

		// FIXME: take these out when we have a real language
		pci_datatypes_init();
		cpuid_datatypes_init();

		DASSERT_MSG(current_context.is_valid(),
				"invalid current_context");
		m_scope = current_context.scope();
	}
	~pp_platform()
	{
	}

	pp_driver *
	find_driver(const string &name)
	{
		// just wrap the existing stuff for now
		return pp_find_driver(name);
	}

	void
	do_discovery()
	{
		// just wrap the existing stuff for now
		pp_do_discovery(m_scope);
	}

	pp_scope *
	scope() const
	{
		return m_scope;
	}

    private:
	//FIXME: make this the context
	pp_scope *m_scope;
};

#endif // PP_PP_PLATFORM_H__
