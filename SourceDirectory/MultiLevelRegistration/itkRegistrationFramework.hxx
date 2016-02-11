/*
insert comments here
*/

#ifndef __itkRegistrationFramework_hxx
#define __itkRegistrationFramework_hxx

#include "itkRegistrationFramework.h"

namespace itk
{
	// member function implementations
	void RegistrationFramework::RegistrationFramework( ImageType::Pointer fixedImage, ImageType::Pointer movingImage )
	{
		this->m_fixedImage = fixedImage;
		this->m_movingImage = movingImage;
	}

} // end namespace

#endif