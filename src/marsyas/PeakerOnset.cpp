/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "PeakerOnset.h"

using namespace std;
using namespace Marsyas;

PeakerOnset::PeakerOnset(string name):MarSystem("PeakerOnset", name)
{
	addControls();
}

PeakerOnset::PeakerOnset(const PeakerOnset& a) : MarSystem(a)
{
	ctrl_onsetWinSize_ = getctrl("mrs_natural/onsetWinSize");
	ctrl_threshold_ = getctrl("mrs_real/threshold");
	ctrl_onsetDetected_ = getctrl("mrs_bool/onsetDetected");
	ctrl_confidence_ = getctrl("mrs_real/confidence");
}

PeakerOnset::~PeakerOnset()
{
}

MarSystem*
PeakerOnset::clone() const
{
	return new PeakerOnset(*this);
}

void
PeakerOnset::addControls()
{
	addctrl("mrs_natural/onsetWinSize", 0, ctrl_onsetWinSize_);
	addctrl("mrs_real/threshold", 0.0, ctrl_threshold_);
	addctrl("mrs_bool/onsetDetected", false, ctrl_onsetDetected_);
	addctrl("mrs_real/confidence", 0.0, ctrl_confidence_);
}

void
PeakerOnset::myUpdate(MarControlPtr sender)
{
	MRSDIAG("PeakerOnset.cpp - PeakerOnset:myUpdate");

	ctrl_onSamples_->setValue(1, NOUPDATE);
	if(inObservations_ > 1)
	{
		MRSWARN("PeakerOnset::myUpdate() - inObservations is bigget than 1. This MarSystem only takes the first observation into consideration...");
	}
	ctrl_onObservations_->setValue(1, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
	ctrl_onObsNames_->setValue("onset_confidence");

	if(inSamples_ < 1 + 2 * ctrl_onsetWinSize_->to<mrs_natural>())
	{
		MRSWARN("PeakerOnset::myUpdate() - inSamples is too small for specified onsetWinSize: onset detection not possible to be performed!");
		ctrl_onsetWinSize_->setValue(0, NOUPDATE);
	}
}

void
PeakerOnset::myProcess(realvec& in, realvec& out)
{
	ctrl_onsetDetected_->setValue(false);
	ctrl_confidence_->setValue(0.0);
	out.setval(0.0);
	
	mrs_natural w = ctrl_onsetWinSize_->to<mrs_natural>();

	if(w == 0)
		return;

	//point to check for an onset
	mrs_natural checkPoint = inSamples_-1-w;
	mrs_real checkPointValue = in(checkPoint);
	mrs_bool isOnset = true;

	//check first condition
	for (t = inSamples_-1; t >= inSamples_-1-2*w ; t--)
	{
		if(checkPointValue < in(t))
		{
			isOnset = false;
			//cout << "failed 1st condition!" << endl;
			break;
		}
	}

	//check second condition
	mrs_real m = 0.0;
	for(t=0; t < inSamples_; t++)
		m += in(t);
	m /= inSamples_;
	if(checkPointValue < (m * ctrl_threshold_->to<mrs_real>()))
	{
		isOnset = false;
		//cout << "failed 2nd condition!" << endl;
	}

	//third condition from Dixon2006 (DAFx paper) is not implemented
	//since it was found on that paper that its impact is minimal...

	if(isOnset)
	{
		ctrl_onsetDetected_->setValue(true);
		ctrl_confidence_->setValue(1.0); //!!!!!!!!!!!!! [!] must still find a way to output a confidence...
		out.setval(1.0);
	}
}


