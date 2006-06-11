/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003 Upinder S. Bhalla and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _tp_wrapper_h
#define _tp_wrapper_h
// class myFlexLexer: public yyFlexLexer
class tp_wrapper: 
	public myFlexLexer, public p_element
{
    public:
		tp_wrapper(const string& n)
		:
		p_element(n),
		readline_md(this, &tp_wrapper::readline_mdi),
		proc_md(this, &tp_wrapper::proc_mdi),
		echo_ms(this, &tp_wrapper::echo_msi)
		{
			;
		}

		// msgdest handlers
		static void handle_readline(element *p, string& a0) {
			static_cast<tp_wrapper *>(p)->AddInput(&a0);
		}

		/*
		void Process() { // Infinite loop for parser
			while (1) {
				// cerr << "lexing... process\n";
				yyparse();
				}
			cerr << "Emerging from Process loop for tparser\n";
		}
		*/

		static void handle_proc(element *p) {
			static_cast<tp_wrapper *>(p)->Process();
		}

		// essential functions
		static element* create_tp_wrapper(
			const string& name, element* pa) {
			return new tp_wrapper(name);
		}
		static void* create_tp() {
			return 0;
		}
		const cinfo* GetCinfo() const {
			return &mycinfo;
		}

		// msgdest definitions
		singlemsgdest readline_md;
		singlemsgdest proc_md;

		// msgsrc definitions
		singlemsgsrc echo_ms;

    private:
		// msgdest template definitions
		static msgdest1<string&> readline_mdi;
		static msgdest0 proc_mdi;
	
		// msgsrc template definitions
		static field1<string&> echo_msi;

		static finfo* fieldlookup[];
		static const cinfo mycinfo;
};
#endif // _tp_wrapper_h
