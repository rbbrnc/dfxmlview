#include <QDebug>

#include "dfxml.h"
#include "volume.h"

DfxmlVolume::DfxmlVolume()
{
	m_blockSize = 0;
	m_byteRuns.clear();
}

void DfxmlVolume::loadFromXmlStream(QXmlStreamReader &stream)
{
	// Let's check that we're really getting a <source> tag.
	if (stream.tokenType() != QXmlStreamReader::StartElement
	    && stream.name() == "volume") {
		return;
	}

	stream.readNext();

	// Loop over <volume> entries until we hit the </volume> EndElement.
	while (!(stream.tokenType() == QXmlStreamReader::EndElement &&
		stream.name() == "volume")) {
		if (stream.tokenType() == QXmlStreamReader::StartElement) {
			if (stream.name() == "block_size") {
				stream.readNext();
				//qDebug() << "<block_size>" << stream.text().toString();
				if (stream.tokenType() == QXmlStreamReader::Characters) {
					setBlockSize(stream.text().toString());
				}
			}
			if (stream.name() == "byte_runs") {
				m_byteRuns = Dfxml::readByteRuns(stream);
			}
	        }
		stream.readNext();
	}
}

void DfxmlVolume::addByteRun(DfxmlByteRun br)
{
	m_byteRuns.append(br);
}

void DfxmlVolume::setBlockSize(const QString &size)
{
	bool ok;
	m_blockSize = size.toULongLong(&ok, 10);
	if (!ok) {
		m_blockSize = 0;
	}
}

quint64 DfxmlVolume::blockSize() const
{
	return m_blockSize;
}

QList<DfxmlByteRun> DfxmlVolume::byteRunList() const
{
	return m_byteRuns;
}

void DfxmlVolume::save(QXmlStreamWriter &stream)
{
	stream.writeStartElement("volume");
	Dfxml::writeByteRuns(m_byteRuns, stream);
	stream.writeTextElement("block_size", QString::number(m_blockSize));
	stream.writeEndElement();
}

#ifndef QT_NO_DEBUG_OUTPUT
void DfxmlVolume::debugDump() const
{
	qDebug() << "----------" << __PRETTY_FUNCTION__ << "----------";
	qDebug() << "block_size:" << m_blockSize;

	for (int i = 0; i < m_byteRuns.count(); i++) {
		m_byteRuns.at(i).debugDump();
	}

	qDebug() << "-------------------------------------------------";
}
#endif


