struct SampleStruct
{
    int value0;
    float value1;
};

RWStructuredBuffer<SampleStruct> rwBuffer0 : register(u0);
RWByteAddressBuffer rwBuffer1 : register(u1);

[numthreads(16, 16, 1)]
void main(int3 gtid : SV_GroupThreadID)
{

}