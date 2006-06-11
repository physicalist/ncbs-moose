#ifndef _Shell_h
#define _Shell_h
class Shell
{
	friend class ShellWrapper;
	public:
		Shell()
		{
		}
		bool splitFieldString( const string& field,
		     string& e, string& f );
		bool splitField( const string& fieldstr, Field& f );
		Element* findElement( const string& path );
		Element* checkWorkingElement( );
		Element* shellRelativeFind( const string& path );

	private:
		int isInteractive_;
		string parser_;
		string workingElement_;
		vector< string > workingElementStack_;
		vector< string > commandHistory_;
};
#endif // _Shell_h
