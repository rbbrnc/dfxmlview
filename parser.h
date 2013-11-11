#ifndef DFXML_PARSER_H
#define DFXML_PARSER_H

#include <QList>
#include <QXmlStreamReader>

#include <QDebug>

#include "byterun.h"
#include "fileobject.h"
#include "source.h"

class DfxmlParser
{
	public:
		DfxmlParser(const QString &xmlFile);

		bool parse();

		QList<DfxmlFileObject> fileObjects() const;
		DfxmlFileObject fileObject(int index) const;
		DfxmlSource source() const;

		QString dataFile() const;

	private:
		QString m_xmlFile;

		QXmlStreamReader m_xml;

		QList<DfxmlFileObject> m_fileObjects;
		DfxmlSource m_source;
};

#endif
