#include <QFile>
#include <QDebug>

#include "parser.h"

DfxmlParser::DfxmlParser(const QString &xmlFile)
{
	m_xmlFile = xmlFile;
}

QList<DfxmlFileObject> DfxmlParser::fileObjects() const
{
	return m_fileObjects;
}

bool DfxmlParser::parse()
{
	QFile file(m_xmlFile);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return false;
	}

	m_xml.setDevice(&file);

	while (!m_xml.atEnd() && !m_xml.hasError()) {
		QXmlStreamReader::TokenType token = m_xml.readNext();

		switch (token) {
		case QXmlStreamReader::NoToken:
			qDebug() << "NoToken";
			break;
		case QXmlStreamReader::Invalid:
			qDebug() << "Invalid";
			break;
		case QXmlStreamReader::StartDocument:
			//qDebug() << "START DOCUMENT" << m_xml.name();
			break;
		case QXmlStreamReader::EndDocument:
			//qDebug() << "END DOCUMENT" << m_xml.name();
			break;
		case QXmlStreamReader::StartElement:
			if (m_xml.name() == "fileobject") {
				DfxmlFileObject obj;
				obj.loadFromXmlStream(m_xml);
				//obj.debugDump();
				if (!obj.fileName().isEmpty()) {
					m_fileObjects.append(obj);
				} else {
					// XXX Handle Errors
				}
			}
			if (m_xml.name() == "source") {
				m_source.loadFromXmlStream(m_xml);
				//m_source.debugDump();
			}
			if (m_xml.name() == "creator") {
				//m_xml.readNext();
			}
			if (m_xml.name() == "configuration") {
				//m_xml.readNext();
			}
			if (m_xml.name() == "metadata") {
				//m_xml.readNext();
			}

			break;
		case QXmlStreamReader::EndElement:
			//qDebug() << "END ELEM" << m_xml.name();
			break;
		case QXmlStreamReader::Characters:
			//qDebug() << "CHAR" << m_xml.name();
			break;
		case QXmlStreamReader::Comment:
			//qDebug() << "COMMENT" << m_xml.name();
			break;
		case QXmlStreamReader::DTD:
			qDebug() << "DTD" << m_xml.name();
			break;
		case QXmlStreamReader::EntityReference:
			qDebug() << "ENTITY REF" << m_xml.name();
			break;
		case QXmlStreamReader::ProcessingInstruction:
			qDebug() << "PROCINS" << m_xml.name();
			break;
		default:
			qDebug() << m_xml.name();
			break;
		}
	}
	file.close();

	if (m_xml.hasError()) {
		qDebug() << "ERROR:" << m_xml.errorString();
		// do error handling
		return false;
	}

	return true;
}

DfxmlSource DfxmlParser::source() const
{
	return m_source;
}

QString DfxmlParser::dataFile() const
{
	return m_source.imageName();
}

DfxmlFileObject DfxmlParser::fileObject(int objnum) const
{
	DfxmlFileObject obj;
	if (objnum < 0 || objnum > m_fileObjects.count()) {
		return obj;
	}

	return m_fileObjects[objnum];
}
