#if 0
; SPIR-V
; Version: 1.6
; Generator: Google spiregg; 0
; Bound: 12
; Schema: 0
               OpCapability Shader
               OpMemoryModel Logical GLSL450
               OpEntryPoint Fragment %main_ps "main_ps" %out_var_SV_Target0
               OpExecutionMode %main_ps OriginUpperLeft
               OpSource HLSL 650
               OpName %out_var_SV_Target0 "out.var.SV_Target0"
               OpName %main_ps "main_ps"
               OpDecorate %out_var_SV_Target0 Location 0
      %float = OpTypeFloat 32
    %float_1 = OpConstant %float 1
    %float_0 = OpConstant %float 0
    %v4float = OpTypeVector %float 4
          %7 = OpConstantComposite %v4float %float_1 %float_0 %float_0 %float_1
%_ptr_Output_v4float = OpTypePointer Output %v4float
       %void = OpTypeVoid
         %10 = OpTypeFunction %void
%out_var_SV_Target0 = OpVariable %_ptr_Output_v4float Output
    %main_ps = OpFunction %void None %10
         %11 = OpLabel
               OpStore %out_var_SV_Target0 %7
               OpReturn
               OpFunctionEnd

#endif

const unsigned char g_debug_main_ps_spirv[] = {
  0x03, 0x02, 0x23, 0x07, 0x00, 0x06, 0x01, 0x00, 0x00, 0x00, 0x0e, 0x00,
  0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x06, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x5f, 0x70, 0x73, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x07, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x05, 0x00, 0x00, 0x00,
  0x8a, 0x02, 0x00, 0x00, 0x05, 0x00, 0x07, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x6f, 0x75, 0x74, 0x2e, 0x76, 0x61, 0x72, 0x2e, 0x53, 0x56, 0x5f, 0x54,
  0x61, 0x72, 0x67, 0x65, 0x74, 0x30, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x5f, 0x70, 0x73, 0x00,
  0x47, 0x00, 0x04, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x20, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x2b, 0x00, 0x04, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x17, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x07, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x07, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x13, 0x00, 0x02, 0x00, 0x09, 0x00, 0x00, 0x00, 0x21, 0x00, 0x03, 0x00,
  0x0a, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x36, 0x00, 0x05, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00,
  0x0b, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x07, 0x00, 0x00, 0x00, 0xfd, 0x00, 0x01, 0x00, 0x38, 0x00, 0x01, 0x00
};
