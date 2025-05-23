[Mesh]
  [rmp]
    type = ReactorMeshParams
    dim = 3
    geom = "Hex"
    assembly_pitch = 7.10315
    axial_regions = '1.0 1.0'
    axial_mesh_intervals = '1 1'
    top_boundary_id = 201
    bottom_boundary_id = 202
  []

  [pin1]
    type = PinMeshGenerator
    reactor_params = rmp
    pin_type = 1
    pitch = 1.42063
    num_sectors = 2
    ring_radii = '0.2'
    duct_halfpitch = '0.68'
    mesh_intervals = '1 1 1'
    quad_center_elements = true
    region_ids='1 2 5; 11 12 15'
  []

  [pin2]
    type = PinMeshGenerator
    reactor_params = rmp
    pin_type = 2
    pitch = 1.42063
    num_sectors = 2
    mesh_intervals = '2'
    region_ids='3; 13'
    quad_center_elements = true
  []

  [amg]
    type = AssemblyMeshGenerator
    assembly_type = 1
    inputs = 'pin1 pin2'
    pattern = '1 1;
              1 0 1;
               1 1'
    background_intervals = 1
    background_region_id = '6 16'
    duct_halfpitch = '3.5'
    duct_region_ids = '7; 17'
    duct_intervals = '1'
    extrude = true
  []
[]

[Executioner]
  type = Steady
[]

[Problem]
  solve = false
[]

[Outputs]
  [out]
    type = Exodus
    execute_on = timestep_end
    output_extra_element_ids = true
    extra_element_ids_to_output = 'region_id'
  []
[]
