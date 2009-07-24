#include <iostream>
#include <fstream>
#include <string>

#include <glob.h>

#include "ast.h"
#include "control.h"
#include "parser.h"
#include "codemodel.h"
#include "binder.h"

using namespace std;

/* Output format:
 *
 * namespace: (<name> (namespace*) (class*))
 * class:     (<name> (class*) (function*))
 * function:  (<return-type> <name> arg*)
 * arg:       (<type> <name>)
 */

void doType(TypeInfo type)
{
	cout << type.toString().toStdString();
}

void doArgument(ArgumentModelItem &item)
{
	string name = item->name().toStdString();

	if(name == "") name = "<unknown>";

	cout << "(";
	doType(item->type());
	cout  << " " << name << ")";
}

void doFunction(FunctionModelItem &item)
{
	cout << "(";
	doType(item->type());
	cout << " " << item->name().toStdString();

	QListIterator<ArgumentModelItem> ai(item->arguments());
	while(ai.hasNext()) {
		cout << " ";
		ArgumentModelItem item = ai.next();
		doArgument(item);
	}

	cout << ")";
}

void doClass(ClassModelItem &item)
{
	cout << "(" << item->name().toStdString() << " (";

	QListIterator<ClassModelItem> ci(item->classMap().values());
	while(ci.hasNext()) {
		ClassModelItem item = ci.next();
		doClass(item);
	}

	cout << ") (";

	QListIterator<FunctionModelItem> fi(item->functionMap().values());
	while(fi.hasNext()) {
		FunctionModelItem item = fi.next();
		doFunction(item);
	}

	cout << ")";
}

void doNamespace(NamespaceModelItem &item)
{
	cout << "(" << item->name().toStdString() << " (";

	QListIterator<NamespaceModelItem> ni(item->namespaceMap().values());
	while(ni.hasNext()) {
		NamespaceModelItem item = ni.next();
		doNamespace(item);
	}

	cout << ") (";

	QListIterator<ClassModelItem> ci(item->classMap().values());
	while(ci.hasNext()) {
		ClassModelItem item = ci.next();
		doClass(item);
	}

	cout << "))";
}

void doFile(FileModelItem &item)
{
	cout << "(top-level " << "(";

	QListIterator<NamespaceModelItem> ni(item->namespaceMap().values());
	while(ni.hasNext()) {
		NamespaceModelItem item = ni.next();
		doNamespace(item);
	}

	cout << ") (";

	QListIterator<ClassModelItem> ci(item->classMap().values());
	while(ci.hasNext()) {
		ClassModelItem item = ci.next();
		doClass(item);
	}

	cout << "))" << endl;
}

int main(int argc, char *argv[])
{
	ifstream f;
	string s, l;

	f.open(argv[1]);
	while(!f.eof()) {
		getline(f, l);
		s += l + "\n";
	}

	Control control;
	Parser p(&control);
	pool poo;

	TranslationUnitAST *ast = p.parse(s.c_str(), s.length(), &poo);

	CodeModel model;
	Binder binder(&model, p.location());
	FileModelItem dom = binder.run(ast);

	doFile(dom);

	return 0;
}
