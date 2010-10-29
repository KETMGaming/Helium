/*#include "Precompile.h"*/
#include "PivotTransform.h"

#include "Foundation/Math/EulerAngles.h"
#include "Foundation/Math/Constants.h"
#include "Foundation/Undo/PropertyCommand.h"

#include "Core/SceneGraph/Scene.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_CLASS( PivotTransform );

void PivotTransform::EnumerateClass( Reflect::Compositor<PivotTransform>& comp )
{
    comp.AddField( (Vector3 PivotTransform::*)&PivotTransform::m_Shear, "m_Shear" );
    comp.AddField( &PivotTransform::m_ScalePivot,                       "m_ScalePivot" );
    comp.AddField( &PivotTransform::m_ScalePivotTranslate,              "m_ScalePivotTranslate" );
    comp.AddField( &PivotTransform::m_RotatePivot,                      "m_RotatePivot" );
    comp.AddField( &PivotTransform::m_RotatePivotTranslate,             "m_RotatePivotTranslate" );
    comp.AddField( &PivotTransform::m_TranslatePivot,                   "m_TranslatePivot" );
    comp.AddField( &PivotTransform::m_SnapPivots,                       "m_SnapPivots" );
}

void PivotTransform::InitializeType()
{
    Reflect::RegisterClassType< SceneGraph::PivotTransform >( TXT( "SceneGraph::PivotTransform" ) );

    PropertiesGenerator::InitializePanel( TXT( "Pivot" ), CreatePanelSignature::Delegate( &PivotTransform::CreatePanel ));
}

void PivotTransform::CleanupType()
{
    Reflect::UnregisterClassType< SceneGraph::PivotTransform >();
}

PivotTransform::PivotTransform() 
: m_SnapPivots( true )
{
}

i32 PivotTransform::GetImageIndex() const
{
    return -1; // Helium::GlobalFileIconsTable().GetIconID( TXT( "transform" ) );
}

tstring PivotTransform::GetApplicationTypeName() const
{
    return TXT( "Group" );
}

Shear PivotTransform::GetShear() const
{
    return m_Shear;
}

void PivotTransform::SetShear(const Shear& value)
{
    m_Shear = value;

    Dirty();
}

Vector3 PivotTransform::GetScalePivot() const
{
    Vector3 result = m_ScalePivot;
    (GetScaleComponent() * GetRotateComponent()).TransformVertex( result );
    return result;
}

void PivotTransform::SetScalePivot(const Vector3& value)
{
    SetScalePivot(value, true);
}

void PivotTransform::SetScalePivot(const Vector3& value, bool snapSiblings)
{
    Vector3 v = value;
    (GetScaleComponent() * GetRotateComponent()).Inverted().TransformVertex( v );

    // old pivot sample
    Vector3 a = Vector3::Zero;

    // new pivot sample
    Vector3 b = Vector3::Zero;

    // build total scale with old pivot
    Matrix4 scaleA = GetScaleComponent();
    scaleA.TransformVertex(a);

    // set the new pivot value
    m_ScalePivot = v;

    // build total scale with new pivot
    Matrix4 scaleB = GetScaleComponent();
    scaleB.TransformVertex(b);

    // compensate for the delta
    m_ScalePivotTranslate = m_ScalePivotTranslate + a - b;

    if (GetSnapPivots() && snapSiblings)
    {
        SetRotatePivot(value, false);
        SetTranslatePivot(value, false);
    }

    Dirty();
}

Vector3 PivotTransform::GetScalePivotTranslate() const
{
    return m_ScalePivotTranslate;
}

void PivotTransform::SetScalePivotTranslate(const Vector3& value)
{
    m_ScalePivotTranslate = value;

    Dirty();
}

Vector3 PivotTransform::GetRotatePivot() const
{
    Vector3 result = m_RotatePivot;
    GetRotateComponent().TransformVertex( result );
    return result;
}

void PivotTransform::SetRotatePivot(const Vector3& value)
{
    SetRotatePivot(value, true);
}

void PivotTransform::SetRotatePivot(const Vector3& value, bool snapSiblings)
{
    Vector3 v = value;
    GetRotateComponent().Inverted().TransformVertex( v );

    // old pivot sample
    Vector3 a = Vector3::Zero;

    // new pivot sample
    Vector3 b = Vector3::Zero;

    // build total rotation with old pivot
    Matrix4 rotateA = GetRotateComponent();
    rotateA.TransformVertex(a);

    // set the new pivot value
    m_RotatePivot = v;

    // build total rotation with new pivot
    Matrix4 rotateB = GetRotateComponent();
    rotateB.TransformVertex(b);

    // compensate for the delta
    m_RotatePivotTranslate = m_RotatePivotTranslate + a - b;

    if (GetSnapPivots() && snapSiblings)
    {
        SetScalePivot(value, false);
        SetTranslatePivot(value, false);
    }

    Dirty();
}

Vector3 PivotTransform::GetRotatePivotTranslate() const
{
    return m_RotatePivotTranslate;
}

void PivotTransform::SetRotatePivotTranslate(const Vector3& value)
{
    m_RotatePivotTranslate = value;

    Dirty();
}

Vector3 PivotTransform::GetTranslatePivot() const
{
    return m_TranslatePivot;
}

void PivotTransform::SetTranslatePivot(const Vector3& value)
{
    SetTranslatePivot(value, true);
}

void PivotTransform::SetTranslatePivot(const Vector3& value, bool snapSiblings)
{
    m_TranslatePivot = value;

    if (GetSnapPivots() && snapSiblings)
    {
        SetScalePivot(value, false);
        SetRotatePivot(value, false);
    }

    Dirty();
}

bool PivotTransform::GetSnapPivots() const
{
    return m_SnapPivots;
}

void PivotTransform::SetSnapPivots(bool value)
{
    m_SnapPivots = value;
}

Matrix4 PivotTransform::GetScaleComponent() const
{
    Matrix4 spi( m_ScalePivot * -1.0f );
    Matrix4 sh( m_Shear );
    Matrix4 sp( m_ScalePivot + m_ScalePivotTranslate );

    return spi * __super::GetScaleComponent() * sh * sp;
}

Matrix4 PivotTransform::GetRotateComponent() const
{
    Matrix4 rpi( m_RotatePivot * -1.0f );
    Matrix4 rp( m_RotatePivot + m_RotatePivotTranslate );

    return rpi * __super::GetRotateComponent() * rp;
}

Matrix4 PivotTransform::GetTranslateComponent() const
{
    return __super::GetTranslateComponent();
}

Undo::CommandPtr PivotTransform::ResetTransform()
{
    // we need to snapshot here to save pivot state
    Undo::CommandPtr command = SnapShot();

    // disregard the base class' command, its not complete
    __super::ResetTransform ();

    m_Shear = Shear::Identity;

    m_ScalePivot = Vector3::Zero;
    m_ScalePivotTranslate = Vector3::Zero;

    m_RotatePivot = Vector3::Zero;
    m_RotatePivotTranslate = Vector3::Zero;

    m_TranslatePivot = Vector3::Zero;

    return command;
}

// outputs
void MatrixDecomposeFixedPivots(Scale& scale, Shear& shear, 
                                EulerAngles& rotate, Vector3& translate, 
                                // inputs 
                                const  Matrix4& totalMatrix, 
                                const Vector3& scalePivot, const Vector3& scalePivotTranslate, 
                                const Vector3& rotatePivot, const Vector3& rotatePivotTranslate, 
                                const Vector3& translatePivot)
{
    // [tfm] = [Sp]-1x[S]x[Sh]x[Sp]x[Spt]x[Rp]-1x[R]x[Rp]x[Rpt]x[T]

    // [tfm] = [Sp]-1x[M1]
    //    M1 =        [S]x[Sh]x[Sp]x[Spt]x[Rp]-1x[R]x[Rp]x[Rpt]x[T]
    //    M1 = [Sp]x[Sp]-1[M1]; 
    //    M1 = [Sp]x[tfm]; 

    Matrix4 M1 = Matrix4(scalePivot) * totalMatrix; 

    //    M1 = [S]x[Sh]x[Sp]x[Spt]x[Rp]-1x[R]x[Rp]x[Rpt]x[T]
    //    M1 = [S]x[Sh]x[M2]; 

    Shear   localShear; 
    Scale   localScale; 
    Matrix3 localRot; 
    Vector3 localTrans; 

    //    M1 = [S]x[Sh]x[localRot]x[localTrans]
    // 
    M1.Decompose(localScale, localShear, localRot, localTrans); 

    //    M2 = [localRot]x[localTrans]
    Matrix4 M2 = Matrix4(localRot) * Matrix4(localTrans); 

    //    M2 = [Sp]x[Spt]x[Rp]-1x[R]x[Rp]x[Rpt]x[T]
    //    M2 = [M3]             x[R]x[M4]
    //    M2 = [M3]             x[M5]

    // we know Sp, Spt, and Rp so get them out of there
    // 
    //    M3  =                   [Sp]x[Spt]x[Rp']
    //     I  = [Rp]x[Spt']x[Sp']x[Sp]x[Spt]x[Rp']
    //     I  = [M3']            x[M3] 
    //    M3' = [Rp]x[Spt']x[Sp'] 
    // 
    //         M2 = [M3]x[M5]; 
    //   [M3']xM2 = [M5]; 
    Matrix4 M3_inv = Matrix4(rotatePivot) * Matrix4(scalePivotTranslate * -1.0f) * Matrix4(scalePivot * -1.0f); 

    Shear   noShear; 
    Scale   noScale; 
    Matrix3 finalRot; 
    Vector3 M4_trans; 

    Matrix4 M5 = M3_inv * M2; 

    // recall: [M5] = [R]x[M4]

    M5.Decompose(noScale, noShear, finalRot, M4_trans); 

    // recall: [M4] = [Rp]x[Rpt]x[T] -- just translation in all of those
    Matrix4 M4(M4_trans); 

    // therefore: [Rpt']x[Rp']x[Rp]x[Rpt]x[T] = [T]
    //            [Rpt']x[Rp']x[M4] = [T]

    Matrix4 T = Matrix4(rotatePivotTranslate * -1.0f) * Matrix4(rotatePivot*-1.0f) * M4; 

    // that's our final translation
    // 
    Vector3 finalTrans(T.t.x, T.t.y, T.t.z); 

    shear = localShear; 
    scale = localScale; 

    rotate = finalRot; 
    translate = finalTrans; 
}

Undo::CommandPtr PivotTransform::ComputeObjectComponents()
{
    const Matrix4& parentGlobalTransform = m_Parent->GetTransform()->GetGlobalTransform();
    const Matrix4& parentInverseGlobalTransform = m_Parent->GetTransform()->GetInverseGlobalTransform();


    //
    // Save pivots into locals and transform them
    //

    Vector3 scalePivot = m_ScalePivot;
    Vector3 rotatePivot = m_RotatePivot;
    Vector3 translatePivot = m_TranslatePivot;
    Vector3 scalePivotTranslate = m_ScalePivotTranslate; 
    Vector3 rotatePivotTranslate = m_RotatePivotTranslate; 

    // take from the base class, but we don't use the base class because we are 
    // decomposing to include shear 
    // 

    Undo::CommandPtr command = ResetTransform();

    Scale scale;
    Shear shear; 
    EulerAngles rotate;
    Vector3 translate;

    Matrix4 nextLocalMatrix; 

    // zero the compensators out. i don't 100% understand the compensators, and 
    // the old code was disregarding them, so i'm zeroing them out as well
    //
    // the code below will work if we want to keep them in the future. 
    //
    scalePivotTranslate = Vector3::Zero; 
    rotatePivotTranslate = Vector3::Zero; 

    if (GetInheritTransform())
    {
        nextLocalMatrix = m_GlobalTransform * m_Parent->GetTransform()->GetInverseGlobalTransform(); 
    }
    else
    {
        nextLocalMatrix = m_GlobalTransform; 
    }

    // decompose the next localMatrix into the parts we don't know...
    MatrixDecomposeFixedPivots(scale, shear, rotate, translate, 
        nextLocalMatrix, 
        scalePivot, scalePivotTranslate, 
        rotatePivot, rotatePivotTranslate, 
        translatePivot); 

    // local vars to member variables 
    //
    m_Shear = shear; 
    m_Scale = scale;
    m_Rotate = rotate;
    m_Translate = translate;


    m_ScalePivot = scalePivot;
    m_RotatePivot = rotatePivot;
    m_TranslatePivot = translatePivot;

    m_ScalePivotTranslate = scalePivotTranslate; 
    m_RotatePivotTranslate = rotatePivotTranslate; 

    return command;
}

Undo::CommandPtr PivotTransform::CenterTransform()
{
    Undo::BatchCommandPtr batch = new Undo::BatchCommand();

    batch->Push( __super::CenterTransform() );

    // if we are not a group or we don't have children then don't bother
    if (!IsGroup() || m_Children.Empty())
    {
        return batch;
    }

    Vector3 pos;
    float w = 1.0f / m_Children.Size();
    for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
    {
        SceneGraph::HierarchyNode* hierarchyNode = *itr;

        const SceneGraph::Transform* transform = hierarchyNode->GetTransform();
        HELIUM_ASSERT( transform );
        if ( transform )
        {
            const Matrix4& globalTransform = transform->GetGlobalTransform();
            Vector3 p = Vector3( globalTransform.t.x, globalTransform.t.y, globalTransform.t.z );
            p *= w;
            pos += p;
        }
    }

    batch->Push( new Undo::PropertyCommand<Matrix4> ( new Helium::MemberProperty<SceneGraph::Transform, Matrix4> (this, &SceneGraph::Transform::GetGlobalTransform, &SceneGraph::Transform::SetGlobalTransform), Matrix4 (pos) ) );

    Evaluate(GraphDirections::Downstream);

    for ( OS_HierarchyNodeDumbPtr::Iterator itr = m_Children.Begin(), end = m_Children.End(); itr != end; ++itr )
    {
        SceneGraph::HierarchyNode* n = *itr;

        SceneGraph::Transform* t = Reflect::ObjectCast<SceneGraph::Transform>( n );

        if (!t)
        {
            continue;
        }

        batch->Push( t->ComputeObjectComponents() );
    }

    Dirty();

    return batch;
}

bool PivotTransform::ValidatePanel(const tstring& name)
{
    if (name == TXT( "Pivot" ) )
    {
        return true;
    }

    return __super::ValidatePanel(name);
}

void PivotTransform::CreatePanel(CreatePanelArgs& args)
{
    args.m_Generator->PushContainer( TXT( "Pivots" ) );

    {
        args.m_Generator->PushContainer();
        static const tstring helpText = TXT( "FIXME: NEEDS HELP" );   
        args.m_Generator->AddLabel( TXT( "Shear" ) )->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetShearYZ, &PivotTransform::SetShearYZ)->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetShearXZ, &PivotTransform::SetShearXZ)->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetShearXY, &PivotTransform::SetShearXY)->a_HelpText.Set( helpText );
        args.m_Generator->Pop();
    }

    {
        args.m_Generator->PushContainer();
        static const tstring helpText = TXT( "FIXME: NEEDS HELP" );
        args.m_Generator->AddLabel( TXT( "Scale Pivot" ) )->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetScalePivotX, &PivotTransform::SetScalePivotX)->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetScalePivotY, &PivotTransform::SetScalePivotY)->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetScalePivotZ, &PivotTransform::SetScalePivotZ)->a_HelpText.Set( helpText );
        args.m_Generator->Pop();
    }

    {
        args.m_Generator->PushContainer();
        static const tstring helpText = TXT( "FIXME: NEEDS HELP" );
        args.m_Generator->AddLabel( TXT( "Scale Pivot Translate" ) )->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetScalePivotTranslateX, &PivotTransform::SetScalePivotTranslateX)->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetScalePivotTranslateY, &PivotTransform::SetScalePivotTranslateY)->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetScalePivotTranslateZ, &PivotTransform::SetScalePivotTranslateZ)->a_HelpText.Set( helpText );
        args.m_Generator->Pop();
    }

    {
        args.m_Generator->PushContainer();
        static const tstring helpText = TXT( "FIXME: NEEDS HELP" );
        args.m_Generator->AddLabel( TXT( "Rotate Pivot" ) )->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetRotatePivotX, &PivotTransform::SetRotatePivotX)->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetRotatePivotY, &PivotTransform::SetRotatePivotY)->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetRotatePivotZ, &PivotTransform::SetRotatePivotZ)->a_HelpText.Set( helpText );
        args.m_Generator->Pop();
    }

    {
        args.m_Generator->PushContainer();
        static const tstring helpText = TXT( "FIXME: NEEDS HELP" );
        args.m_Generator->AddLabel( TXT( "Rotate Pivot Translate" ) )->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetRotatePivotTranslateX, &PivotTransform::SetRotatePivotTranslateX)->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetRotatePivotTranslateY, &PivotTransform::SetRotatePivotTranslateY)->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetRotatePivotTranslateZ, &PivotTransform::SetRotatePivotTranslateZ)->a_HelpText.Set( helpText );
        args.m_Generator->Pop();
    }

    {
        args.m_Generator->PushContainer();
        static const tstring helpText = TXT( "FIXME: NEEDS HELP" );
        args.m_Generator->AddLabel( TXT( "Translate Pivot" ) )->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetTranslatePivotX, &PivotTransform::SetTranslatePivotX)->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetTranslatePivotY, &PivotTransform::SetTranslatePivotY)->a_HelpText.Set( helpText );
        args.m_Generator->AddValue<SceneGraph::PivotTransform, f32>(args.m_Selection, &PivotTransform::GetTranslatePivotZ, &PivotTransform::SetTranslatePivotZ)->a_HelpText.Set( helpText );
        args.m_Generator->Pop();
    }

    {
        args.m_Generator->PushContainer();
        static const tstring helpText = TXT( "FIXME: NEEDS HELP" );
        args.m_Generator->AddLabel( TXT( "Snap Pivots" ) )->a_HelpText.Set( helpText );
        args.m_Generator->AddCheckBox<SceneGraph::PivotTransform, bool>(args.m_Selection, &PivotTransform::GetSnapPivots, &PivotTransform::SetSnapPivots)->a_HelpText.Set( helpText );
        args.m_Generator->Pop();
    }

    args.m_Generator->Pop();
}

f32 PivotTransform::GetShearYZ() const
{
    return GetShear().yz;
}

void PivotTransform::SetShearYZ(f32 shear)
{
    Shear s = GetShear();
    s.yz = shear;
    SetShear(s);
}

f32 PivotTransform::GetShearXZ() const
{
    return GetShear().xz;
}

void PivotTransform::SetShearXZ(f32 shear)
{
    Shear s = GetShear();
    s.xz = shear;
    SetShear(s);
}

f32 PivotTransform::GetShearXY() const
{
    return GetShear().xy;
}

void PivotTransform::SetShearXY(f32 shear)
{
    Shear s = GetShear();
    s.xy = shear;
    SetShear(s);
}

f32 PivotTransform::GetScalePivotX() const
{
    return GetScalePivot().x;
}

void PivotTransform::SetScalePivotX(f32 translate)
{
    Vector3 s = GetScalePivot();
    s.x = translate;
    SetScalePivot(s);
}

f32 PivotTransform::GetScalePivotY() const
{
    return GetScalePivot().y;
}

void PivotTransform::SetScalePivotY(f32 translate)
{
    Vector3 s = GetScalePivot();
    s.y = translate;
    SetScalePivot(s);
}

f32 PivotTransform::GetScalePivotZ() const
{
    return GetScalePivot().z;
}

void PivotTransform::SetScalePivotZ(f32 translate)
{
    Vector3 s = GetScalePivot();
    s.z = translate;
    SetScalePivot(s);
}

f32 PivotTransform::GetScalePivotTranslateX() const
{
    return GetScalePivotTranslate().x;
}

void PivotTransform::SetScalePivotTranslateX(f32 translate)
{
    Vector3 s = GetScalePivotTranslate();
    s.x = translate;
    SetScalePivotTranslate(s);
}

f32 PivotTransform::GetScalePivotTranslateY() const
{
    return GetScalePivotTranslate().y;
}

void PivotTransform::SetScalePivotTranslateY(f32 translate)
{
    Vector3 s = GetScalePivotTranslate();
    s.y = translate;
    SetScalePivotTranslate(s);
}

f32 PivotTransform::GetScalePivotTranslateZ() const
{
    return GetScalePivotTranslate().z;
}

void PivotTransform::SetScalePivotTranslateZ(f32 translate)
{
    Vector3 s = GetScalePivotTranslate();
    s.z = translate;
    SetScalePivotTranslate(s);
}
f32 PivotTransform::GetRotatePivotX() const
{
    return GetRotatePivot().x;
}

void PivotTransform::SetRotatePivotX(f32 translate)
{
    Vector3 s = GetRotatePivot();
    s.x = translate;
    SetRotatePivot(s);
}

f32 PivotTransform::GetRotatePivotY() const
{
    return GetRotatePivot().y;
}

void PivotTransform::SetRotatePivotY(f32 translate)
{
    Vector3 s = GetRotatePivot();
    s.y = translate;
    SetRotatePivot(s);
}

f32 PivotTransform::GetRotatePivotZ() const
{
    return GetRotatePivot().z;
}

void PivotTransform::SetRotatePivotZ(f32 translate)
{
    Vector3 s = GetRotatePivot();
    s.z = translate;
    SetRotatePivot(s);
}

f32 PivotTransform::GetRotatePivotTranslateX() const
{
    return GetRotatePivotTranslate().x;
}

void PivotTransform::SetRotatePivotTranslateX(f32 translate)
{
    Vector3 s = GetRotatePivotTranslate();
    s.x = translate;
    SetRotatePivotTranslate(s);
}

f32 PivotTransform::GetRotatePivotTranslateY() const
{
    return GetRotatePivotTranslate().y;
}

void PivotTransform::SetRotatePivotTranslateY(f32 translate)
{
    Vector3 s = GetRotatePivotTranslate();
    s.y = translate;
    SetRotatePivotTranslate(s);
}

f32 PivotTransform::GetRotatePivotTranslateZ() const
{
    return GetRotatePivotTranslate().z;
}

void PivotTransform::SetRotatePivotTranslateZ(f32 translate)
{
    Vector3 s = GetRotatePivotTranslate();
    s.z = translate;
    SetRotatePivotTranslate(s);
}

f32 PivotTransform::GetTranslatePivotX() const
{
    return GetTranslatePivot().x;
}

void PivotTransform::SetTranslatePivotX(f32 translate)
{
    Vector3 s = GetTranslatePivot();
    s.x = translate;
    SetTranslatePivot(s);
}

f32 PivotTransform::GetTranslatePivotY() const
{
    return GetTranslatePivot().y;
}

void PivotTransform::SetTranslatePivotY(f32 translate)
{
    Vector3 s = GetTranslatePivot();
    s.y = translate;
    SetTranslatePivot(s);
}

f32 PivotTransform::GetTranslatePivotZ() const
{
    return GetTranslatePivot().z;
}

void PivotTransform::SetTranslatePivotZ(f32 translate)
{
    Vector3 s = GetTranslatePivot();
    s.z = translate;
    SetTranslatePivot(s);
}
