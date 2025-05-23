[Mesh]
  [square_1]
    type = PolygonConcentricCircleMeshGenerator
    num_sides = 4
    num_sectors_per_side = '2 2 2 2'
    background_intervals = 1
    ring_radii = 3.0
    ring_intervals = 1
    ring_block_ids = '10'
    ring_block_names = 'center_tri_1'
    background_block_ids = 20
    background_block_names = background
    polygon_size = 5.0
    preserve_volumes = on
    flat_side_up = true
  []
  [square_2]
    type = PolygonConcentricCircleMeshGenerator
    num_sides = 4
    num_sectors_per_side = '2 2 2 2'
    background_intervals = 1
    ring_radii = 2.5
    ring_intervals = 1
    ring_block_ids = '40'
    ring_block_names = 'center_tri_2'
    background_block_ids = 20
    background_block_names = background
    polygon_size = 5.0
    preserve_volumes = on
    flat_side_up = true
  []
  [pattern]
    type = PatternedCartesianMeshGenerator
    inputs = 'square_1 square_2'
    pattern = '0 0 0 0;
               0 1 0 0;
               0 0 1 0;
               0 0 0 0'
    square_size = 48
    duct_sizes = '21 22 23'
    duct_intervals = '2 2 2'
    rotate_angle = 0
    duct_block_ids = '2000 2100 2200'
    background_block_id = 1500
  []
  [pattern_unused]
    type = PatternedCartesianMeshGenerator
    inputs = 'square_1 square_2'
    pattern = '0 0 0 0;
               0 1 0 0;
               0 0 1 0;
               0 0 0 0'
    square_size = 48
    duct_sizes = '21 22 23'
    duct_intervals = '2 2 2'
    rotate_angle = 0
    duct_block_ids = '2000 2100 2200'
    background_block_id = 1500
  []
  [pattern_2]
    type = PatternedCartesianMeshGenerator
    inputs = 'pattern pattern_unused'
    pattern = '0 0;
               0 0'
    pattern_boundary = none
    generate_core_metadata = true
  []
[]

[Outputs]
  [out]
    type = Exodus
    output_extra_element_ids = false
  []
[]
