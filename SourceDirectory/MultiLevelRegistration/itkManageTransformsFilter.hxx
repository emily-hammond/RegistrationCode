#ifndef __itkManageTransformsFilter_hxx
#define __itkManageTransformsFilter_hxx_hxx

#include "itkManageTransformsFilter.h"

namespace itk
{
	// constructor
	ManageTransformsFilter::ManageTransformsFilter()
	{
		this->m_compositeTransform = CompositeTransformType::New();
	}

	void ManageTransformsFilter::AddTransform( TransformType::Pointer transform )
	{
		this->m_compositeTransform->AddTransform( transform );

		return;
	}

	void ManageTransformsFilter::Print()
	{
		std::cout << std::endl;
		std::cout << this->m_compositeTransform << std::endl;
		return;
	}
	
} // end namespace

#endif