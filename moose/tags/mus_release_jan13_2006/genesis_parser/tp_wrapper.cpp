/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003 Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include <setjmp.h>
#include <FlexLexer.h>
#include "script.h"
#include "tp.h"
#include "tp_wrapper.h"

msgdest1<string&> tp_wrapper::readline_mdi(
	"readline_info", &tp_wrapper::handle_readline);
msgdest0 tp_wrapper::proc_mdi(
	"proc_info", &tp_wrapper::handle_proc);
field1<string&> tp_wrapper::echo_msi("echo_info");

finfo* tp_wrapper::fieldlookup[] =
{
	new destlist_f< singlemsgdest, tp_wrapper >( "readline",
		&tp_wrapper::readline_md),
	new destlist_f< singlemsgdest, tp_wrapper >( "proc",
		&tp_wrapper::proc_md),
	new srclist_f< singlemsgsrc, tp_wrapper >( "echo",
		&tp_wrapper::echo_ms),
};

const cinfo tp_wrapper::mycinfo(
	"tp",
	"Automatically generated",
	"tp: description here",
	"p_element",
	tp_wrapper::fieldlookup,
	sizeof(tp_wrapper::fieldlookup)/sizeof(finfo *),
	&tp_wrapper::create_tp_wrapper,
	&tp_wrapper::create_tp
);
