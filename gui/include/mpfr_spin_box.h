/*
 *  mpfr_spin_box.h -- part of FractalNow
 *
 *  Copyright (c) 2012 Marc Pegon <pe.marc@free.fr>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
 
 /**
  * \file mpfr_spin_box.h
  * \brief Header file introducing MPFRSpinBox class.
  *
  * \author Marc Pegon
  */

#ifndef __MPFR_SPIN_BOX_H__
#define __MPFR_SPIN_BOX_H__

#include <mpfr.h>

#include <QAbstractSpinBox>

class MPFRSpinBox : public QAbstractSpinBox
{
	Q_OBJECT

	public:
	MPFRSpinBox(QWidget * parent = 0);
	~MPFRSpinBox();

	enum Notation {
		ClassicNotation = 0,
		ScientificNotation
	};
	void fixup ( QString & input ) const;
	void stepBy(int step);
	const mpfr_t *value();
	long double value_d();
	void setMaximum ( const mpfr_t * max );
	void setMinimum ( const mpfr_t * min );
	void setRange ( const mpfr_t * min, const mpfr_t * max);
	void setMaximum ( long double max );
	void setMinimum ( long double min );
	void setRange ( long double min, long double max);
	void setSingleStep ( const mpfr_t * val );
	void setSingleStep ( long double val );
	void setNotation ( Notation notation );
	void setDecimals ( int prec );

	/* Checks that the str can be converted to a long */
	QValidator::State validate ( QString & input, int & pos ) const;

	protected:
	QAbstractSpinBox::StepEnabled stepEnabled() const;
	virtual mpfr_t *valueFromText(const QString &text) const;
	virtual QString textFromValue(const mpfr_t *val) const;

	protected:
	mpfr_t m_value;
	mpfr_t m_singleStep;
	mpfr_t m_minimum;
	mpfr_t m_maximum;
	Notation m_notation;
	int m_decimals;

	private:
	void updateText();
	inline bool isInRange(const mpfr_t val) const;
	void aux_setValue(const mpfr_t *val);
	void setValue(const mpfr_t *val, bool updateText);
	void aux_setValue(long double val);
	void aux_setMinimum();
	void aux_setMaximum();

	public slots:
	void setValue(const mpfr_t *val);
	void setValue(long double val);

	private slots:
	void onTextEdited(const QString & text);
	void onEditingFinished();

	signals:
	void valueChanged ( const mpfr_t *val );
};

#endif

