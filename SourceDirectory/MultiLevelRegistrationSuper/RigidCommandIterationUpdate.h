#include "itkVersorTransformOptimizer.h"

class RigidCommandIterationUpdate: public itk::Command
{
public:
	typedef RigidCommandIterationUpdate	Self;
	typedef itk::Command				Superclass;
	typedef itk::SmartPointer< Self >	Pointer;
	itkNewMacro( Self );
protected:
	RigidCommandIterationUpdate():
		m_observe( false ),
		m_debug( false),
		m_DebugDirectory( "" )
	{
		std::cout << "\n\nObserver initialized\n\n" << std::endl;
	};
	bool m_observe;
	bool m_debug;
	std::string m_DebugDirectory;
public:
	typedef itk::VersorTransformOptimizer				OptimizerType;
	typedef const OptimizerType *						OptimizerPointer;
	void Observe() { this->m_observe = !this->m_observe; }
	void Debug(std::string debugDirectory) { this->m_debug = !this->m_debug; this->m_DebugDirectory = debugDirectory; }
	void Execute( itk::Object *caller, const itk::EventObject &event )
	{
		Execute( (const itk::Object *)caller, event);
	}
	void Execute( const itk::Object * object, const itk::EventObject &event )
	{
		OptimizerPointer	optimizer = static_cast< OptimizerPointer >( object );
		if( ! itk::IterationEvent().CheckEvent( &event ) )
		{
			return;
		}
		if( (optimizer->GetCurrentIteration()%10 == 1 || optimizer->GetCurrentIteration()%10 == 2) && this->m_observe )
		{
			std::cout << optimizer->GetCurrentIteration() << " " << optimizer->GetCurrentStepLength();// << " " << optimizer->GetGradientMagnitude();
			std::cout << " " << optimizer->GetValue() << " " << optimizer->GetCurrentPosition();
			std::cout << std::endl;
		}
		if ((optimizer->GetCurrentIteration()%50 == 1) && this->m_debug)
		{
			std::string filename = this->m_DebugDirectory + "Transform_" + std::to_string(optimizer->GetCurrentIteration()) + ".tfm";
			itk::ScaleVersor3DTransform< double >::Pointer transform = itk::ScaleVersor3DTransform< double >::New();
			transform->SetParameters( optimizer->GetCurrentPosition() );

			WriteOutTransform< itk::ScaleVersor3DTransform< double >>(filename.c_str(), transform);
		}
	}
};