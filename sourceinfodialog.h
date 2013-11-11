#ifndef SOURCEINFODIALOG_H
#define SOURCEINFODIALOG_H

#include <QDialog>

class DfxmlListModel;
class DfxmlSource;

namespace Ui {
	class SourceInfoDialog;
}

class SourceInfoDialog : public QDialog
{
	Q_OBJECT

	public:
		explicit SourceInfoDialog(DfxmlListModel *model, QWidget *parent = 0);
		~SourceInfoDialog();

	private slots:
		void on_buttonBox_accepted();
		void on_buttonBox_rejected();
		void updateAcquisitionDateTime();

	private:
		Ui::SourceInfoDialog *ui;

		DfxmlSource    *m_source;

		bool m_acquisitionDateTimeChanged;
};

#endif
