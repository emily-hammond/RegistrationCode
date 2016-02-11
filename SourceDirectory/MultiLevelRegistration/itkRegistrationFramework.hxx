/*
insert comments here
*/

#ifndef __itkRegistrationFramework_hxx
#define __itkRegistrationFramework_hxx

#include "itkRegistrationFramework.h"

namespace itk
{
	// member function implementations
	void RegistrationFramework::SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage )
	{
		this->m_fixedImage = fixedImage;
		this->m_movingImage = movingImage;

		return;
	}
/*
	void RegistrationFramework::SetInitialTransform( std::list< TransformType::Pointer > * initialTransform )
	{
		this->m_transforms = CompositeTransformType::New();

		std::list< TransformType::Pointer >::const_iterator it = initialTransform->begin();
		while( it !=initialTransform->end() )
		{
			this->m_transforms->AddTransform( (*it).GetPointer() );
		}

		return;
	}*/

} // end namespace

#endif