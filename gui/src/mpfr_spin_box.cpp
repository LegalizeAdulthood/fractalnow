/*
 *  mpfr_spin_box.cpp -- part of FractalNow
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

#include "mpfr_spin_box.h"

#include <QLineEdit>

MPFRSpinBox::MPFRSpinBox(QWidget * parent) : QAbstractSpinBox(parent)
{
	m_notation = ClassicNotation;
	m_decimals = -1;

	mpfr_init(m_value);
	mpfr_set_ui(m_value, 0, MPFR_RNDN);
	mpfr_init(m_singleStep);
	mpfr_set_ui(m_singleStep, 1, MPFR_RNDN);

	mpfr_init(m_minimum);
	mpfr_init(m_maximum);
	mpfr_set_ui_2exp(m_maximum, 1, 99.99, MPFR_RNDN);
	mpfr_set_ui(m_minimum, 0, MPFR_RNDN);

	mpfr_t initValue;
	mpfr_init(initValue);
	mpfr_set_d(initValue, 0.5, MPFR_RNDN);
	setValue(&initValue);
	mpfr_clear(initValue);

	connect(lineEdit(), SIGNAL(textEdited(const QString &)),
		this, SLOT(onTextEdited(const QString &)));
	connect(lineEdit(), SIGNAL(editingFinished()), this, SLOT(onEditingFinished()));
}

MPFRSpinBox::~MPFRSpinBox()
{
	mpfr_clear(m_value);
	mpfr_clear(m_singleStep);
	mpfr_clear(m_minimum);
	mpfr_clear(m_maximum);
}

void MPFRSpinBox::aux_setMaximum()
{
	/* Update m_value and/or m_minimum if needed after maximum has changed. */
	if (mpfr_cmp(m_maximum, m_minimum) < 0) {
		mpfr_set(m_minimum, m_maximum, MPFR_RNDN);
	}
	if (mpfr_cmp(m_value, m_minimum) < 0) {
		setValue(&m_minimum);
	}
}

void MPFRSpinBox::setMaximum ( const mpfr_t * max )
{
	mpfr_set(m_maximum, *max, MPFR_RNDN);
	aux_setMaximum();
}

void MPFRSpinBox::setMaximum ( long double max )
{
	mpfr_set_ld(m_maximum, max, MPFR_RNDN);
	aux_setMaximum();
}

void MPFRSpinBox::aux_setMinimum()
{
	/* Update m_value and/or m_maximum if needed after minimum has changed. */
	if (mpfr_cmp(m_minimum, m_maximum) > 0) {
		mpfr_set(m_maximum, m_minimum, MPFR_RNDN);
	}
	if (mpfr_cmp(m_value, m_maximum) > 0) {
		setValue(&m_maximum);
	}
}

void MPFRSpinBox::setMinimum ( const mpfr_t * min )
{
	mpfr_set(m_minimum, *min, MPFR_RNDN);
	aux_setMinimum();
}

void MPFRSpinBox::setMinimum ( long double min )
{
	mpfr_set_ld(m_minimum, min, MPFR_RNDN);
	aux_setMinimum();
}

void MPFRSpinBox::setRange ( const mpfr_t * min, const mpfr_t * max)
{
	setMinimum(min);
	setMaximum(max);
}

void MPFRSpinBox::setRange ( long double min, long double max )
{
	setMinimum(min);
	setMaximum(max);
}

void MPFRSpinBox::setSingleStep ( const mpfr_t * val )
{
	mpfr_set(m_singleStep, *val, MPFR_RNDN);
}

void MPFRSpinBox::setSingleStep ( long double val )
{
	mpfr_set_ld(m_singleStep, val, MPFR_RNDN);
}

void MPFRSpinBox::setNotation ( Notation notation )
{
	if (notation != m_notation) {
		m_notation = notation;
		updateText();
	}
}

void MPFRSpinBox::setDecimals ( int prec )
{
	if (prec != m_decimals) {
		m_decimals = prec;
		updateText();
	}
}

void MPFRSpinBox::updateText()
{
	lineEdit()->setText(textFromValue(&m_value));
}

void MPFRSpinBox::fixup( QString & input ) const
{
	Q_UNUSED(input);
}

void MPFRSpinBox::stepBy(int step)
{
	mpfr_t newValue;
	mpfr_init(newValue);
	mpfr_mul_si(newValue, m_singleStep, step, MPFR_RNDN);
	mpfr_add(newValue, newValue, m_value, MPFR_RNDN);

	setValue(&newValue);

	mpfr_clear(newValue);
}

const mpfr_t *MPFRSpinBox::value()
{
	return &m_value;
}

long double MPFRSpinBox::value_d()
{
	return mpfr_get_ld(m_value, MPFR_RNDN);
}

inline bool MPFRSpinBox::isInRange(const mpfr_t val) const
{
	return (mpfr_cmp(val,m_minimum) >= 0 && mpfr_cmp(val,m_maximum) <= 0);
}

inline void MPFRSpinBox::aux_setValue(const mpfr_t *val)
{
	bool changed = (mpfr_cmp(m_value, *val) != 0);
	mpfr_set(m_value, *val, MPFR_RNDN);
	if (changed) {
		emit valueChanged(&m_value);
	}
}

inline void MPFRSpinBox::setValue(const mpfr_t *val, bool updateText)
{
	if (mpfr_cmp(*val, m_minimum) < 0) {
		aux_setValue(&m_minimum);
	} else if (mpfr_cmp(*val, m_maximum) > 0) {
		aux_setValue(&m_maximum);
	} else {
		aux_setValue(val);
	}

	if (updateText) this->updateText();
}

void MPFRSpinBox::setValue(const mpfr_t *val)
{
	setValue(val, true);
}

inline void MPFRSpinBox::aux_setValue(long double val)
{
	bool changed = (mpfr_cmp_ld(m_value, val) != 0);
	mpfr_set_ld(m_value, val, MPFR_RNDN);
	if (changed) {
		emit valueChanged(&m_value);
	}
}

void MPFRSpinBox::setValue(long double val)
{
	if (mpfr_cmp_ld(m_minimum, val) > 0) {
		aux_setValue(&m_minimum);
	} else if (mpfr_cmp_ld(m_maximum, val) < 0) {
		aux_setValue(&m_maximum);
	} else {
		aux_setValue(val);
	}

	this->updateText();
}

QValidator::State MPFRSpinBox::validate ( QString & input, int & pos ) const
{
	Q_UNUSED(pos);
	FILE *tmp = tmpfile();
	fputs(input.toStdString().c_str(), tmp);
	rewind(tmp);
	mpfr_t dst;
	mpfr_init(dst);
	QValidator::State res = QValidator::Invalid;
	if (mpfr_inp_str(dst, tmp, 10, MPFR_RNDN) != 0 && isInRange(dst)) {
		res = QValidator::Acceptable;
	}

	mpfr_clear(dst);
	fclose(tmp);

	return res;
}

mpfr_t *MPFRSpinBox::valueFromText(const QString &text) const
{
	FILE *tmp = tmpfile();
	fputs(text.toStdString().c_str(), tmp);
	rewind(tmp);
	mpfr_t *res = (mpfr_t *)malloc(sizeof(mpfr_t));
	mpfr_init(*res);
	mpfr_inp_str(*res, tmp, 10, MPFR_RNDN);

	return res;
}

QString MPFRSpinBox::textFromValue(const mpfr_t *val) const
{
	char *str;
	switch(m_notation) {
	case ClassicNotation:
		mpfr_asprintf(&str, "%.*Rf", (m_decimals < 0) ?
			mpfr_get_prec(*val) : m_decimals, *val);
		break;
	case ScientificNotation:
		if (m_decimals < 0) {
			mpfr_asprintf(&str, "%RE", *val);
		} else {
			mpfr_asprintf(&str, "%.*RE", m_decimals, *val);
		}
		break;
	}
	QString res(str);
	mpfr_free_str(str);

	return res;
}

QAbstractSpinBox::StepEnabled MPFRSpinBox::stepEnabled() const
{
	StepEnabled res = 0;

	if (mpfr_cmp(m_value, m_minimum) > 0) {
		res |= StepDownEnabled;
	}
	if (mpfr_cmp(m_value, m_maximum) < 0) {
		res |= StepUpEnabled;
	}

	return res;
}

void MPFRSpinBox::onTextEdited(const QString & text)
{
	mpfr_t *value = valueFromText(text);
	setValue(value, false);
	mpfr_clear(*value);
	free(value);
}

void MPFRSpinBox::onEditingFinished()
{
	updateText();
}

