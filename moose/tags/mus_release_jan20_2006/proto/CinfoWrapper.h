#ifndef _CinfoWrapper_h
#define _CinfoWrapper_h
class CinfoWrapper: 
	public Cinfo, public Neutral
{
    public:
		CinfoWrapper(const string& n)
		:
			Neutral( n )
		{
			;
		}
///////////////////////////////////////////////////////
//    Field header definitions.                      //
///////////////////////////////////////////////////////
		static string getName( const Element* e ) {
			return static_cast< const CinfoWrapper* >( e )->name_;
		}
		static string getAuthor( const Element* e ) {
			return static_cast< const CinfoWrapper* >( e )->author_;
		}
		static string getDescription( const Element* e ) {
			return static_cast< const CinfoWrapper* >( e )->description_;
		}
		static string getBaseName( const Element* e ) {
			return static_cast< const CinfoWrapper* >( e )->baseName_;
		}
		static string getFields(
			const Element* e, unsigned long index );
///////////////////////////////////////////////////////
// Msgsrc header definitions .                       //
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// dest header definitions .                         //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Synapse creation and info access functions.       //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Conn access functions.                            //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class creation and info access functions.         //
///////////////////////////////////////////////////////
		static Element* create(
			const string& name, Element* pa, const Element* proto ) {
			// Put tests for parent class here
			// Put proto initialization stuff here
			// const Cinfo* p = dynamic_cast<const Cinfo *>(proto);
			// if (p)... and so on. 
			return new CinfoWrapper(name);
		}

		const Cinfo* cinfo() const {
			return &cinfo_;
		}


    private:
///////////////////////////////////////////////////////
// MsgSrc template definitions.                      //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Synapse definition.                               //
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Static initializers for class and field info      //
///////////////////////////////////////////////////////
		static Finfo* fieldArray_[];
		static const Cinfo cinfo_;
};
#endif // _CinfoWrapper_h
