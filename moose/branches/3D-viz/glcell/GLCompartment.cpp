#include <osg/Vec3>
#include <osg/ref_ptr>
#include <osg/Geometry>

#include "GLCompartment.h"

osg::Vec3 GLCompartment::rotateTranslatePoint( osg::Vec3 position, osg::Quat& quatRotation, osg::Vec3& translation )
{
	position = quatRotation * position;

	position[0] += translation[0];
	position[1] += translation[1];
	position[2] += translation[2];

	return position;
}
