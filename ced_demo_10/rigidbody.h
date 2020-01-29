#include "ced_matrix.h"
#include "ced_view.h"

#ifndef RIGIDBODY_H_23487239847234
#define RIGIDBODY_H_23487239847234

#pragma pack(push, 1)
	// The following structure stores forces information for a 3D rigid body. These change almost every frame during runtime
	struct SForce3 {
		::ced::SCoord3<float>							Velocity						= {};
		::ced::SCoord3<float>							Acceleration					= {};
		::ced::SCoord3<float>							Rotation						= {};
	};

	// The following structure stores mass information for a 3D rigid body. These almost never change during runtime
	struct SMass3 {
		float											LinearDamping					= 1.0f;
		float											AngularDamping					= 1.0f;
		float											InverseMass						= 0;
		::ced::SMatrix3<float>							InverseAngularMassTensor		= {1,0,0,0,1,0,0,0,1};
	};

	struct STransform3 {
		::ced::SQuaternion	<float>						Orientation						= {0, 0, 0, 1};
		::ced::SCoord3		<float>						Position						= {};
	};

	struct SRigidBodyFlags {
		bool											OutdatedTransform				: 1;
		bool											OutdatedTensorWorld				: 1;	// Tell the object that our matrices are up to date
		bool											Active							: 1;

		constexpr										SRigidBodyFlags					()
			: OutdatedTransform		(true)
			, OutdatedTensorWorld	(true)
			, Active				(true)
		{}
	};

	struct SRigidBodyFrame {
		::ced::SMatrix3<float>							InverseInertiaTensorWorld		= {1, 0, 0, 0, 1, 0, 0, 0, 1};
		::ced::SCoord3<float>							LastFrameAcceleration			= {0, 0, 0};
		::ced::SCoord3<float>							AccumulatedForce				= {0, 0, 0};
		::ced::SCoord3<float>							AccumulatedTorque				= {0, 0, 0};

		int32_t											ClearAccumulators				()	{ AccumulatedForce = AccumulatedTorque = {}; return 0; }
	};
#pragma pack(pop)
	void											updateTransform					(::STransform3 & bodyTransform, ::ced::SMatrix4<float> & transformLocal);
	int32_t											integrateForces					(double duration, ::SRigidBodyFrame& bodyFrame, ::SForce3 & bodyForce, const ::SMass3 & bodyMass);
	int32_t											integratePosition				(double duration, double durationHalfSquared, ::SRigidBodyFlags& bodyFlags, ::STransform3 & bodyTransform, const ::SForce3 & bodyForces);
	void											transformInertiaTensor			(::ced::SMatrix3<float> & iitWorld, const ::ced::SMatrix3<float> &iitBody, const ::ced::SMatrix4<float> &rotmat);

	struct SIntegrator3 {
		::ced::container<::SRigidBodyFrame	>			BodyFrames						= {};
		::ced::container<::SRigidBodyFlags	>			BodyFlags						= {};
		::ced::container<::SForce3			>			Forces							= {};
		::ced::container<::SMass3			>			Masses							= {};
		::ced::container<::STransform3		>			Transforms						= {};
		::ced::container<::ced::SMatrix4<float>	>		TransformsLocal					= {};

		static constexpr const ::ced::SMatrix4<float>	MatrixIdentity4					= {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		static constexpr const ::ced::SMatrix3<float>	MatrixIdentity3					= {1,0,0,0,1,0,0,0,1};

		int32_t 										Spawn							()	{
			BodyFrames				.push_back({});
			BodyFlags				.push_back({});
			Forces					.push_back({});
			Masses					.push_back({});
			Transforms				.push_back({});
			return TransformsLocal	.push_back(MatrixIdentity4);
		}

		int32_t 										Spawn							(uint32_t bodyCount)	{
			BodyFrames		.resize(BodyFrames		.size() + bodyCount);
			BodyFlags		.resize(BodyFlags		.size() + bodyCount);
			Forces			.resize(Forces			.size() + bodyCount);
			Masses			.resize(Masses			.size() + bodyCount);
			Transforms		.resize(Transforms		.size() + bodyCount);
			TransformsLocal	.resize(TransformsLocal	.size() + bodyCount);
			return TransformsLocal.size();
		}

		int32_t 										GetTransform					(uint32_t iBody, ::ced::SMatrix4<float>	& transform)	{
			::SRigidBodyFlags									& bodyFlags						= BodyFlags			[iBody];
			::ced::SMatrix4<float>								& bodyTransformLocal			= TransformsLocal	[iBody];
			const bool											requiresUpdate					= bodyFlags.OutdatedTransform || bodyFlags.OutdatedTensorWorld;
			if(requiresUpdate) {
				if(bodyFlags.OutdatedTransform) {
					::STransform3										& bodyTransform					= Transforms[iBody];
					::updateTransform(bodyTransform, bodyTransformLocal);
					bodyFlags.OutdatedTransform						= false;
				}
				if(bodyFlags.OutdatedTensorWorld) {
					::SRigidBodyFrame									& bodyFrame						= BodyFrames[iBody];
					::SMass3											& bodyMass						= Masses	[iBody];
					::transformInertiaTensor(bodyFrame.InverseInertiaTensorWorld, bodyMass.InverseAngularMassTensor, bodyTransformLocal);
					bodyFlags.OutdatedTensorWorld					= false;
				}
			}
			transform										= bodyTransformLocal;
			return requiresUpdate ? 1 : 0;
		}
		void											AddForceAtPoint					(uint32_t iBody, const ::ced::SCoord3<float>& force, ::ced::SCoord3<float> point)			{
			::STransform3										& bodyTransform					= Transforms[iBody];
			::SRigidBodyFlags									& bodyFlags						= BodyFlags	[iBody];
			::SRigidBodyFrame									& bodyFrame						= BodyFrames[iBody];
			point											-= bodyTransform.Position;	// Convert to coordinates relative to center of mass.
			bodyFrame.AccumulatedForce 						+= force;
			bodyFrame.AccumulatedTorque						+= point.Cross( force );
			bodyFlags.Active								= true;
		}
		int32_t											Integrate						(double duration)	{
			const double										durationHalfSquared				= duration * duration * 0.5;
			for(uint32_t iBody = 0; iBody < BodyFlags.size(); ++iBody) {
				::SRigidBodyFlags									& bodyFlags						= BodyFlags[iBody];
				if(false == bodyFlags.Active)
					continue;
				::SRigidBodyFrame									& bodyFrame						= BodyFrames[iBody];
				::SForce3											& bodyForces					= Forces	[iBody];
				::SMass3											& bodyMass						= Masses	[iBody];
				::STransform3										& bodyTransform					= Transforms[iBody];
				::integrateForces	(duration, bodyFrame, bodyForces, bodyMass);
				::integratePosition(duration, durationHalfSquared, bodyFlags, bodyTransform, bodyForces);
			}
			return 0;
		}
		void											SetPosition						(uint32_t iBody, const ::ced::SCoord3<float>& newPosition )			{
			::STransform3										& bodyTransform					= Transforms[iBody];
			if( 0 == memcmp( &newPosition.x, &bodyTransform.Position.x, sizeof( ::ced::SCoord3<float> ) ) )
				return;
			bodyTransform.Position							= newPosition;
			::SRigidBodyFlags									& bodyFlags						= BodyFlags[iBody];
			bodyFlags.OutdatedTransform						=
			bodyFlags.OutdatedTensorWorld					= true;
		}
		void											SetOrientation					(uint32_t iBody, const ::ced::SQuaternion<float>& newOrientation )	{
			::STransform3										& bodyTransform					= Transforms[iBody];
			if( 0 == memcmp( &newOrientation.x, &bodyTransform.Orientation.x, sizeof(::ced::SQuaternion<float>) ) )
				return;
			bodyTransform.Orientation						= newOrientation;
			::SRigidBodyFlags									& bodyFlags						= BodyFlags[iBody];
			bodyFlags.OutdatedTransform						=
			bodyFlags.OutdatedTensorWorld					= true;
		}
	};

#endif // RIGIDBODY_H_23487239847234
