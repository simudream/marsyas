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

#include "NormMaxMin.h"

using namespace std;
using namespace Marsyas;

NormMaxMin::NormMaxMin(string name):MarSystem("NormMaxMin",name)
{

  init_ = false;

  addControls();			
  prev_mode_ = "predict";
}

NormMaxMin::NormMaxMin(const NormMaxMin& a): MarSystem(a) 
{
  prev_mode_ = "predict";
  
  lowerPtr_ = getctrl("mrs_real/lower");
  upperPtr_ = getctrl("mrs_real/upper");
  initPtr_ = getctrl("mrs_bool/init");
  maximumsPtr_ = getctrl("mrs_realvec/maximums");
  minimumsPtr_ = getctrl("mrs_realvec/minimums");
}

NormMaxMin::~NormMaxMin()
{
}

MarSystem* 
NormMaxMin::clone() const 
{
  return new NormMaxMin(*this);
}

void 
NormMaxMin::addControls()
{
  addctrl("mrs_real/lower", 0.0, lowerPtr_);
  addctrl("mrs_real/upper", 1.0, upperPtr_);
  maximums_.create(1);
  minimums_.create(1);
  addctrl("mrs_realvec/maximums", maximums_, maximumsPtr_);
  addctrl("mrs_realvec/minimums", minimums_, minimumsPtr_);
  setctrlState("mrs_realvec/maximums", true);
  setctrlState("mrs_realvec/minimums", true);
  addctrl("mrs_string/mode", "train");
	
  addctrl("mrs_bool/ignoreLast", false);
  addctrl("mrs_bool/init", false, initPtr_);
  setctrlState("mrs_bool/init", true);
}

void
NormMaxMin::myUpdate(MarControlPtr sender)
{
  MRSDIAG("NormMaxMin.cpp - NormMaxMin:myUpdate");

  MarSystem::myUpdate(sender);
  
  inObservations_ = ctrl_inObservations_->to<mrs_natural>();
  
  init_ = initPtr_->to<mrs_bool>();
  
  mrs_natural msize = maximumsPtr_->to<mrs_realvec>().getSize();
  mrs_natural nsize = maximums_.getSize();
  
  if (msize != nsize) 
    {
      maximums_.stretch(msize);
      minimums_.stretch(msize);
    }
  
  if (!init_)
    {
      maximums_.stretch(inObservations_);
      minimums_.stretch(inObservations_);
      
      maximums_.setval(DBL_MIN);
      minimums_.setval(DBL_MAX);
      maximumsPtr_->setValue(maximums_, NOUPDATE);
      minimumsPtr_->setValue(minimums_, NOUPDATE);  
    }
}

void 
NormMaxMin::myProcess(realvec& in, realvec& out)
{
  init_ = true;
  setctrl(initPtr_, init_);
  
  lower_ = lowerPtr_->to<mrs_real>();
  upper_ = upperPtr_->to<mrs_real>();
  
  mode_ = getctrl("mrs_string/mode")->to<mrs_string>();
  mrs_bool ignoreLast = getctrl("mrs_bool/ignoreLast")->to<mrs_bool>();
  
  if (lower_ > upper_) 
    {
      MRSWARN("Lower is greater than upper");
      return;
    }
  
  range_ = upper_ - lower_;
  
  
  if ((prev_mode_ == "predict") && (mode_ == "train"))
    {
      cout << "NormMaxMin - Init" << endl;
      maximums_.setval(DBL_MIN);
      minimums_.setval(DBL_MAX);
      maximumsPtr_->setValue(maximums_, NOUPDATE);
      minimumsPtr_->setValue(minimums_, NOUPDATE);  
    }
  
  if (mode_ == "train")
    {
      cout << "NormMaxMin - Train" << endl;
      // first pass calculate min/max limits
      for (o=0; o < inObservations_; o++)
	for (t = 0; t < inSamples_; t++)
	  {
	    if (in(o,t) > maximums_(o))
	      maximums_(o) = in(o,t);
	    if (in(o,t) < minimums_(o))	
	      minimums_(o) = in(o,t);
	    out(o,t) = in(o,t);
	    
	  }
      
      setctrl(maximumsPtr_, maximums_);
      setctrl(minimumsPtr_, minimums_);  
    }


  if ((prev_mode_ == "train")&&(mode_ == "predict"))	
    {
      cout << "NormMaxMin - Changing from train to predict" << endl;
      maximums_ = maximumsPtr_->to<mrs_realvec>();
      minimums_ = minimumsPtr_->to<mrs_realvec>();
    }
  

  if (mode_ == "predict")
    {
      cout << "NormMaxMin - Predict" << endl;
      // second pass for normalization 
      for (o=0; o < inObservations_; o++)
	for (t = 0; t < inSamples_; t++)
	  {
	    cout << "minimums_(o) " << minimums_(o) << endl;
	    cout << "maximums_(o) " << maximums_(o) << endl;
	    
	    if (ignoreLast) 
	      {
		if (o < inObservations_-1)
		  out(o,t) = lower_ + range_ * ((in(o,t) - minimums_(o)) / (maximums_(o) - minimums_(o)));
		else 
		  out(o,t) = in(o,t);
	      }
	    else
	      {
		out(o,t) = lower_ + range_ * ((in(o,t) - minimums_(o)) / (maximums_(o) - minimums_(o)));
	      }
	  }
    }
  
  

  prev_mode_ = mode_;
}









