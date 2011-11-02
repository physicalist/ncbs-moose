# This code is meant to be run after loading the model in 'Axon.g'.
# 'Axon.g' loads 2 identical copies of an linear, passive neuron.
#
# These are helper functions to scale passive parameters Cm, Rm, Ra,
# diameter and length. By default the second copy ('/axon1') is
# modified.
#
# After scaling a passive parameter, run simulation again, and compare
# plots for '/axon' and '/axon1'.

path = '/axon1'
wildcard = path + '/#[TYPE=Compartment]'

# Temporarily assuming that 'pymoose.py' is not accessible, and copying
# over the function needed from there. If pymoose.py is accessible for
# you, feel free to remove the try/except clause and the 'tweak_field'
# function.
try:
	from pymoose import tweak_field
except ImportError:
	def tweak_field(moose_wildcard, field, assignment_string):
		"""Tweak a specified field of all objects that match the
		moose_wildcard using assignment string. All identifiers in
		assignment string must be fields of the target object.
		
		Example:
		
		tweak_field('/mycell/##[Class=Compartment]', 'Rm', '1.5 / (3.1416 * diameter * length')
		
		will assign Rm to every compartment in mycell such that the
		specific membrane resistance is 1.5 Ohm-m2.
		"""
		from moose import context, Neutral, Compartment
		
		if not isinstance(moose_wildcard, str):
			raise TypeError('moose_wildcard must be a string.')
		id_list = context.getWildcardList(moose_wildcard, True)
		expression = parser.expr(assignment_string)
		expr_list = expression.tolist()
		# This is a hack: I just tried out some possible syntax trees and
		# hand coded the replacement such that any identifier is replaced
		# by moose_obj.identifier
		def replace_fields_with_value(x):
			if len(x)>1:
				if x[0] == symbol.power and x[1][0] == symbol.atom and x[1][1][0] == token.NAME:
					field = x[1][1][1]
					x[1] = [symbol.atom, [token.NAME, 'moose_obj']]
					x.append([symbol.trailer, [token.DOT, '.'], [token.NAME, field]])
				for item in x:
					if isinstance(item, list):
						replace_fields_with_value(item)
			return x
		tmp = replace_fields_with_value(expr_list)
		new_expr = parser.sequence2st(tmp)
		code = new_expr.compile()
		for moose_id in id_list:
			moose_obj = eval('%s(moose_id)' % (Neutral(moose_id).className))
			value = eval(code)
			context.setField(moose_id, field, str(value))

def scale_cm( scale ):
	tweak_field( wildcard, 'Cm', '{0} * Cm'.format( scale ) )

def scale_ra( scale ):
	tweak_field( wildcard, 'Ra', '{0} * Ra'.format( scale ) )

def scale_rm( scale ):
	tweak_field( wildcard, 'Rm', '{0} * Rm'.format( scale ) )

def scale_diameter( scale ):
	tweak_field( wildcard, 'diameter', '{0} * diameter'.format( scale ) )
	
	cm_scale = scale
	tweak_field( wildcard, 'Cm', '{0} * Cm'.format( cm_scale ) )
	
	rm_scale = 1.0 / scale
	tweak_field( wildcard, 'Rm', '{0} * Rm'.format( rm_scale ) )
	
	ra_scale = 1.0 / ( scale * scale )
	tweak_field( wildcard, 'Ra', '{0} * Ra'.format( ra_scale ) )

def scale_length( scale ):
	tweak_field( wildcard, 'length', '{0} * length'.format( scale ) )
	
	cm_scale = scale
	tweak_field( wildcard, 'Cm', '{0} * Cm'.format( cm_scale ) )
	
	rm_scale = 1.0 / scale
	tweak_field( wildcard, 'Rm', '{0} * Rm'.format( rm_scale ) )
	
	ra_scale = scale
	tweak_field( wildcard, 'Ra', '{0} * Ra'.format( ra_scale ) )
