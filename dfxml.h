#ifndef DFXML_H
#define DFXML_H

#include <QList>
#include <QXmlStreamReader>

#include "byterun.h"

namespace Dfxml {
	QList<DfxmlByteRun> readByteRuns(QXmlStreamReader &stream);
	void writeByteRuns(const QList<DfxmlByteRun> &byteRuns, QXmlStreamWriter &stream);
}


#endif
