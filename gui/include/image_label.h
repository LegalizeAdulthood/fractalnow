#ifndef _IMAGE_LABEL__H__
#define _IMAGE_LABEL__H__

#include <QImage>
#include <QLabel>
#include <QPaintEvent>

class ImageLabel : public QLabel
{
	Q_OBJECT
	 
	public:
	QImage *image;
	void paintEvent(QPaintEvent *event);

	private:

	public slots:
	void setImage(QImage *newImage);
};

#endif
