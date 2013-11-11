#include "dfxml.h"

QList<DfxmlByteRun> Dfxml::readByteRuns(QXmlStreamReader &stream)
{
	QList<DfxmlByteRun> byteRuns;

	// Let's check that we're really getting a fileobject.
	if (stream.tokenType() != QXmlStreamReader::StartElement
	    && stream.name() == "byte_runs") {
		return byteRuns;
	}

	stream.readNext();

	// We're going to loop over the things because the order might change.
	//  We'll continue the loop until we hit an EndElement named byte_runs.
	while (!(stream.tokenType() == QXmlStreamReader::EndElement &&
		stream.name() == "byte_runs")) {
		if (stream.tokenType() == QXmlStreamReader::StartElement) {
			// We've found file name.
			if (stream.name() == "byte_run") {
				byteRuns.append(DfxmlByteRun(stream.attributes()));
			}
	        }
		stream.readNext();
	}
	return byteRuns;
}

void Dfxml::writeByteRuns(const QList<DfxmlByteRun> &byteRuns, QXmlStreamWriter &stream)
{
	stream.writeStartElement("byte_runs");
	for (int i = 0; i < byteRuns.count(); i++) {
		stream.writeStartElement("byte_run");
		stream.writeAttribute("offset", QString::number(byteRuns.at(i).offset()));
		stream.writeAttribute("img_offset", QString::number(byteRuns.at(i).imageOffset()));
		stream.writeAttribute("len", QString::number(byteRuns.at(i).size()));
		stream.writeEndElement();
	}
	stream.writeEndElement();
}
