#include "itkVersorTransformOptimizer.h"

class RigidCommandIterationUpdate: public itk::Command
{
public:
	typedef RigidCommandIterationUpdate	Self;
	typedef itk::Command				Superclass;
	typedef itk::SmartPointer< Self >	Pointer;
	itkNewMacro( Self );
protected:
	RigidCommandIterationUpdate() {};
public:
	//typedef itk::RegularStepGradientDescentOptimizer	OptimizerType;
	//typedef itk::VersorRigid3DTransformOptimizer		OptimizerType;
	typedef itk::VersorTransformOptimizer				OptimizerType;
	typedef const OptimizerType *						OptimizerPointer;
	bool m_observe;
	bool m_debug;
	std::string m_DebugDirectory;
	void Observe(bool observeFlag) { m_observe = observeFlag; }
	void Debug(std::string debugDirectory) { m_debug = 1; m_DebugDirectory = debugDirectory; }
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
		if (optimizer->GetCurrentIteration() % (optimizer->GetNumberOfIterations() / 10 == 1) && this->m_debug)
		{
			std::string filename = this->m_DebugDirectory + "\\Transform_" + std::to_string(optimizer->GetCurrentIteration()) + ".tfm";
			itk::ScaleVersor3DTransform< double >::Pointer transform = itk::ScaleVersor3DTransform< double >::New();
			transform->SetParameters( optimizer->GetCurrentPosition() );

			WriteOutTransform< itk::ScaleVersor3DTransform< double >>(filename.c_str(), transform);
		}
	}
};