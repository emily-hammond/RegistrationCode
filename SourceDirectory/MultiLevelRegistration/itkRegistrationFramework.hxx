/*
insert comments here
*/

#ifndef __itkRegistrationFramework_hxx
#define __itkRegistrationFramework_hxx

#include "itkRegistrationFramework.h"

namespace itk
{
	RegistrationFramework::RegistrationFramework()
	{
		 this->m_transforms = CompositeTransformType::New();
	}

	// member function implementations
	void RegistrationFramework::SetImages( ImageType::Pointer fixedImage, ImageType::Pointer movingImage )
	{
		this->m_fixedImage = fixedImage;
		this->m_movingImage = movingImage;
		//this->m_transforms = CompositeTransformType::New();
		std::cout << "Images set." << std::endl;
		return;
	}

	void RegistrationFramework::SetInitialTransform( RigidTransformType::Pointer initialTransform )
	{
		std::cout << "Rigid transform" << std::endl;
		this->m_transforms->AddTransform( initialTransform );
		m_transforms->Print(std::cout);
		return;
	}

	void RegistrationFramework::SetInitialTransform( CompositeTransformType::Pointer initialTransform )
	{
		std::cout << "Composite transform" << std::endl;
		this->m_transforms->AddTransform( initialTransform );
		m_transforms->Print(std::cout);
		return;
	}

} // end namespace

#endif