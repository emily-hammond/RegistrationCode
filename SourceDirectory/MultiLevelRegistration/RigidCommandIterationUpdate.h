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
		if( optimizer->GetCurrentIteration()%10 == 1 || optimizer->GetCurrentIteration()%10 == 2 )
		{
			std::cout << optimizer->GetCurrentIteration() << " " << optimizer->GetCurrentStepLength();// << " " << optimizer->GetGradientMagnitude();
			std::cout << " " << optimizer->GetValue() << " " << optimizer->GetCurrentPosition();
			std::cout << std::endl;
		}
	}
};