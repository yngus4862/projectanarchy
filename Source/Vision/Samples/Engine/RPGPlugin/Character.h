/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_CHARACTER_H__
#define RPG_PLUGIN_CHARACTER_H__

#include <Vision/Samples/Engine/RPGPlugin/Attachment.h>
#include <Vision/Samples/Engine/RPGPlugin/CharacterActionData.h>
#include <Vision/Samples/Engine/RPGPlugin/CharacterStats.h>
#include <Vision/Samples/Engine/RPGPlugin/DamageableEntity.h>
#include <Vision/Samples/Engine/RPGPlugin/Effect.h>
#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/InventoryHandler.h>
#include <Vision/Samples/Engine/RPGPlugin/InventoryItem.h>

class vHavokBehaviorComponent;
typedef VSmartPtr<vHavokBehaviorComponent> vHavokBehaviorComponentPtr;

class RPG_ControllerComponent;
typedef VSmartPtr<RPG_ControllerComponent> RPG_ControllerComponentPtr;

class vHavokCharacterController;
typedef VSmartPtr<vHavokCharacterController> vHavokCharacterControllerPtr;

class hkpCharacterProxy;

enum RPG_CharacterEffect_e
{
  FX_None=-1,
  FX_Spawn=0,
  FX_Ambient,   // Ambient effects will start as soon as the character goes live and persist until the character dies.
  FX_Moving,      // Moving effects start when the character moves, and stop when the character stops moving.
  FX_Footstep,
  FX_MeleeBasicAttackSwing,
  FX_MeleeBasicAttack,
  FX_RangedAttackChargeLoop,
  FX_RangedAttackFire,
  FX_RangedAttackProjectile,
  FX_RangedAttackImpact,
  FX_PowerAttackSwing,
  FX_PowerAttack,
  FX_AoeAttackBegin,
  FX_AoeAttackChargeLoop,
  FX_AoeAttackFire,
  FX_CastHealChargeLoop,
  FX_CastHealOther,
  FX_CastHealSelf,
  FX_HealProjectile,
  FX_HealProjectileImpact,
  FX_ReceiveHeal,
  FX_ReceiveMana,
  FX_TakeHit,
  FX_Block,
  FX_Dodge,
  FX_Challenge,
  FX_Die,
  FX_DeathDissolve,
  FX_Count
};

namespace RPG_CharacterAnimationEvent
{
  enum Enum
  {
    // Common
    kTakeControl,
    kDie,

    kMove,
    kMoveEnd,

    kMeleeAttack,
    kMeleeAttackEnd,

    kRangedAttack,
    kRangedAttackEnd,

    kAoeAttack,
    kAoeAttackEnd,

    kPowerAttack,
    kPowerAttackEnd,

    kHeal,
    kHealEnd,

    // Common timeline
    kSpawnEffect,
    kFootStepEffect,
    kMeleeAttackFire,
    kRangedAttackFire,
    kAoeAttackFire,
    kPowerAttackFire,
    kHealFire,

    // Non-player
    kChallenge,
    kChallengeEnd,

    kNumCharacterAnimationEvents
  };
}

namespace RPG_CharacterAnimationVariable
{
  enum Enum
  {
    // Common
    kMoveSpeed,

    kNumCharacterAnimationVariables
  };
};

/// Character class. Both players and NPC characters should be instantiated with this type
class RPG_Character : public RPG_DamageableEntity
{
public:
  // Constructor needs to be public for FORCE_LINKDYNCLASS on mobile
  RPG_Character();

  // @TODO: make abstract
  virtual VType* GetControllerComponentType() { return NULL; }

  // RPG_BaseEntity
  void CalcPositionForTargeting(hkvVec3& targetOut) const HKV_OVERRIDE;

#ifdef SUPPORTS_SNAPSHOT_CREATION
  void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif

  void Initialize() HKV_OVERRIDE;
  void InitializeProperties() HKV_OVERRIDE;
  void PostInitialize() HKV_OVERRIDE;
  void DisposeObject() HKV_OVERRIDE;
  void ThinkFunction() HKV_OVERRIDE;

  //@{
  // Update methods
  // TODO - implement both server and client tick
  virtual void ServerTick(float deltaTime);
  //virtual void ClientTick(float deltaTime);
  //@}

  void SetVisible(bool visible);

  hkvVec3 const GetEyePosition() const;

  //@{
  // Local Control Methods
  virtual bool GetLocallyControlled() const;
  //@}

  //@{
  // Inventory
  const RPG_InventoryHandler& GetInventoryHandler() const;
  RPG_InventoryHandler& GetInventoryHandler();
  //@}

  //@{
  // Stats
  const RPG_CharacterStats& GetCharacterStats() const;
  RPG_CharacterStats& GetCharacterStats();
  //@}

  //@{
  // Action Data
  const RPG_CharacterActionData& GetCharacterActionData() const;
  RPG_CharacterActionData& GetCharacterActionData();
  //@}

  //@{
  // Combat
  void DoMeleeAttack(RPG_DamageableEntity *targetEntity);

  void DoRangedAttack(hkvVec3 const& targetPoint);

  void DoAoeAttack();

  void DoPowerAttack();

  virtual int TakeDamage(const int damageAmount, const hkvVec3& impactVelocity = hkvVec3(0,0,0)) HKV_OVERRIDE;
  virtual void ApplyKnockBack(const hkvVec3& knockBackVector, const float duration) HKV_OVERRIDE;
  int AddHealth(const int healthAmount);
  void AddMana(const int manaAmount);

  virtual void Die();
  bool IsDead() const;
  bool IsFeigningDeath() const;
  virtual void SpawnDeathPickups();
  const hkvVec3& GetDeathImpulse() const;
  //@}

  vHavokBehaviorComponent *GetBehaviorComponent();
  vHavokBehaviorComponent const *GetBehaviorComponent() const;

  int GetIdForAnimationEvent(RPG_CharacterAnimationEvent::Enum animationEvent) const { return m_animationEventIds[animationEvent]; }
  void RaiseAnimationEvent(RPG_CharacterAnimationEvent::Enum animationEvent);
  void SetAnimationVariable(RPG_CharacterAnimationVariable::Enum animationVariable, float value);

  vHavokCharacterController *GetHavokCharacterControllerComponent() { return m_characterController; }

  //@{
  // AI
  void SetController(RPG_ControllerComponent *newController);
  RPG_ControllerComponent *GetController();
  RPG_ControllerComponent const *GetController() const;
  float GetAggroRadius() const;
  bool IsOnNavMesh(float const testRadius = 1.f) const;
  //@}

  //@{
  // Inventory and inventory-derived values
  int   GetArmor() const;
  bool  IsShieldEquipped() const;
  //@}

  //@{
  // Attachments
  VisBaseEntity_cl* AttachEquipmentPrefab(RPG_EquipmentSlot_e slot, const VString& prefabName);
  VisBaseEntity_cl* AttachEquipmentModel(RPG_EquipmentSlot_e slot, const VString& modelName);
  void DetachEquipment(RPG_EquipmentSlot_e slot, bool disposeObject = true);
  VString const& GetProjectileMeshFilename() const;
  void SetWeaponTrailEnabledForEquippedWeapon(bool const enabled);
  void DetachEquipmentOnDeath();
  //@}

  //@{
  // Effects
  void CreateCharacterEffect(RPG_CharacterEffect_e effectType);
  VisParticleEffect_cl *GetPersistentCharacterEffect(RPG_CharacterEffect_e effectType) const;
  void PauseCharacterEffect(RPG_CharacterEffect_e effectType);
  void FinishCharacterEffect(RPG_CharacterEffect_e effectType);

  // TODO it is not necessary to store character effect definitions per-instance (though see ActivateCharacterEffectDebugDisplay)
  RPG_EffectDefinition const& GetCharacterEffectDefinition(RPG_CharacterEffect_e effectType) const { return m_characterEffectDefinitions[effectType]; }
  //@}

  //@{
  // Movement
  void Teleport(hkvVec3 const& position);
  //@}

  //@{
  // Teams
  void SetTeam(RPG_Team_e const& team);
  RPG_Team_e GetTeam() const;
  //@}

  //@{
  // Debugging
  BOOL ShouldDisplayDebugStateInformation() const { return m_debugDisplayStateInformation; }
  //@}

protected:
  //@{
  // Inventory
  void SetUpDefaultEquipment();
  //@}

  //@{
  // Stats
  void UpdateHealthRegeneration(float const deltaTime);
  void UpdateManaRegeneration(float const deltaTime);
  //@}

  //@{
  // States
  void UpdateDeathState(float const deltaTime);
  //@}

  //@{
  // Effects
  virtual void InitializeCharacterEffects() {}
  void StopAllPersistentCharacterEffects();
  void ActivateCharacterEffectDebugDisplay();
  //@}

  //@{
  // Animation
  void InitAnimationEventIds();
  void InitAnimationVariableIds();

  void OnHavokAnimationEvent(hkbEvent const& behaviorEvent, bool raisedBySdk);

  void ProcessAnimationEvents();
  virtual void OnProcessAnimationEvent(hkbEvent const& behaviorEvent);

  void UpdateBehaviorWorldFromModel();
  //@}

  // VisTypedEngineObject_cl
  void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB ) HKV_OVERRIDE;

protected:
  float m_eyeHeight;

  float m_sensorSize;
  float m_desiredSpeed;
  float m_aggroRadius;

  float m_knockBackStartTime;
  float m_knockBackDuration;
  hkvVec3 m_knockBackVector;

  //@{
  // Death
  bool m_dead;                                            ///< is this character dead?
  bool m_feigningDeath;                                   ///< is this character simulating death?
  float m_timeOfDeath;
  float m_deathDissolveDelay;
  hkvVec3 m_deathImpulse;
  //@}

  //@{
  // Actions
  RPG_CharacterActionData m_actionData;                   ///< Contains information about this character's actions
  //@}

  //@{
  // Inventory
  RPG_InventoryHandler m_inventoryHandler;                ///< Manages creation, deletion and use of this character's inventoryItems
  //@}

  //@{
  // Stats
  RPG_CharacterStats m_stats;                             ///< Contains all stats information
  BOOL  m_displayHealth;                                  ///< @todo: to be replaced by proper UI. Displays character health when true.
  //@}

  //@{
  // Behavior animation
  int m_animationEventIds[RPG_CharacterAnimationEvent::kNumCharacterAnimationEvents];
  int m_animationVariableIds[RPG_CharacterAnimationVariable::kNumCharacterAnimationVariables];

  vHavokBehaviorComponentPtr m_havokBehavior;
  hkArray<hkbEvent> m_queuedAnimationEvents;
  //@}

  //@{
  // Controller
  RPG_ControllerComponentPtr m_controller;
  //@}

  //@{
  // Havok Character Proxy
  vHavokCharacterControllerPtr m_characterController;
  //@}

  //@{
  // Attachments
  VRefCountedCollection<RPG_Attachment> m_equipmentAttachments;
  VString m_equipmentAttachmentBoneNames[ES_Count];
  VString m_defaultEquipment[ES_Count];
  VString m_defaultEquipmentScripts[ES_Count];        ///< alternate default equipment that can be defined by scripts instead of prefabs
  VString m_projectileMeshFilename;
  //@}

  //@{
  // Effects
  RPG_EffectDefinition m_characterEffectDefinitions[FX_Count];    ///< specifiers for visual and sound effects
  RPG_Effect* m_characterEffects[FX_Count];                       ///< instantiated visual and sound effects
  //@}

  //@{
  // Teams
  RPG_Team_e m_defaultTeam;
  RPG_Team_e m_team;
  //@}

  //@{
  // Pickups
  VString m_healthPickupScriptName;
  int m_numHealthPickupsDroppedOnDeathMin;
  int m_numHealthPickupsDroppedOnDeathMax;
  VString m_manaPickupScriptName;
  int m_numManaPickupsDroppedOnDeathMin;
  int m_numManaPickupsDroppedOnDeathMax;
  //@}

  //@{
  // Debugging
  BOOL  m_debugDisplayStats;                              ///< DEBUG. Displays character datasheet when true.
  BOOL  m_debugDisplayMovementInfo;
  BOOL  m_debugDisplayInventory;
  BOOL  m_debugDisplayBehaviorEvents;
  BOOL  m_debugDisplayNavmeshStatus;
  BOOL  m_debugDisplayEffectInformation;
  BOOL  m_debugDisplayStateInformation;
  //@}

private:
  V_DECLARE_SERIAL_DLLEXP(RPG_Character, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_Character, RPG_PLUGIN_IMPEXP);
};

// TODO find an appropriate place for this
namespace RPG_CharacterUtil
{
  int CalcOutgoingDamage(RPG_Character const *character, float damageMultiplier = 1.0f);

  float CalcImpactSpeed(RPG_Character const *character, float impactSpeedMultiplier = 1.0f);
}

#endif

/*
 * Havok SDK - Base file, BUILD(#20131218)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
