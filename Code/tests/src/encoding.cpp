#include "catch.hpp"
#include <Messages/ClientMessageFactory.h>
#include <Messages/AuthenticationRequest.h>
#include <Messages/AuthenticationResponse.h>
#include <Messages/CancelAssignmentRequest.h>
#include <Messages/RemoveCharacterRequest.h>
#include <Messages/AssignCharacterRequest.h>
#include <Structs/ActionEvent.h>
#include <Structs/Mods.h>
#include <Structs/FullObjects.h>
#include <Structs/Objects.h>
#include <Structs/Scripts.h>
#include <Structs/GameId.h>
#include <Structs/Vector3_NetQuantize.h>
#include <Structs/Vector2_NetQuantize.h>
#include <Structs/Rotator2_NetQuantize.h>

#include <Math.hpp>

using namespace TiltedPhoques;

TEST_CASE("Encoding factory", "[encoding.factory]")
{
    Buffer buff(1000);

    AuthenticationRequest request;
    request.Token = "TesSt";

    Buffer::Writer writer(&buff);
    request.Serialize(writer);

    Buffer::Reader reader(&buff);

    const ClientMessageFactory factory;
    auto pMessage = factory.Extract(reader);

    REQUIRE(pMessage);
    REQUIRE(pMessage->GetOpcode() == request.GetOpcode());

    auto pRequest = CastUnique<AuthenticationRequest>(std::move(pMessage));
    REQUIRE(pRequest->Token == request.Token);
}

TEST_CASE("Static structures", "[encoding.static]")
{
    GIVEN("FullObjects")
    {
        FullObjects sendObjects, recvObjects;
        sendObjects.Data.push_back(42);
        sendObjects.Data.push_back(13);

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendObjects.Serialize(writer);

            Buffer::Reader reader(&buff);
            recvObjects.Deserialize(reader);

            REQUIRE(sendObjects == recvObjects);
        }
    }

    GIVEN("Objects")
    {
        Objects sendObjects, recvObjects;
        sendObjects.Data.push_back(42);
        sendObjects.Data.push_back(13);

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendObjects.Serialize(writer);

            Buffer::Reader reader(&buff);
            recvObjects.Deserialize(reader);

            REQUIRE(sendObjects == recvObjects);
        }
    }

    GIVEN("Scripts")
    {
        Scripts sendObjects, recvObjects;
        sendObjects.Data.push_back(42);
        sendObjects.Data.push_back(13);

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendObjects.Serialize(writer);

            Buffer::Reader reader(&buff);
            recvObjects.Deserialize(reader);

            REQUIRE(sendObjects == recvObjects);
        }
    }

    GIVEN("GameId")
    {
        GameId sendObjects, recvObjects;
        sendObjects.ModId = 1456987;
        sendObjects.BaseId = 0x789654;

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendObjects.Serialize(writer);

            Buffer::Reader reader(&buff);
            recvObjects.Deserialize(reader);

            REQUIRE(sendObjects == recvObjects);
        }
    }

    GIVEN("Vector3_NetQuantize")
    {
        Vector3_NetQuantize sendObjects, recvObjects;
        sendObjects.m_x = 142.56f;
        sendObjects.m_y = 45687.7f;
        sendObjects.m_z = -142.56f;

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendObjects.Serialize(writer);

            Buffer::Reader reader(&buff);
            recvObjects.Deserialize(reader);

            REQUIRE(sendObjects == recvObjects);
        }
    }

    GIVEN("Vector2_NetQuantize")
    {
        Vector2_NetQuantize sendObjects, recvObjects;
        sendObjects.X = 1000.89f;
        sendObjects.Y = -485632.75f;

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendObjects.Serialize(writer);

            Buffer::Reader reader(&buff);
            recvObjects.Deserialize(reader);

            REQUIRE(sendObjects == recvObjects);
        }
    }

    GIVEN("Rotator2_NetQuantize")
    {
        Rotator2_NetQuantize sendObjects, recvObjects;
        sendObjects.X = 1.89f;
        sendObjects.Y = TiltedPhoques::Pi * 2.0;

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendObjects.Serialize(writer);

            Buffer::Reader reader(&buff);
            recvObjects.Deserialize(reader);

            REQUIRE(sendObjects == recvObjects);
        }
    }

    GIVEN("Rotator2_NetQuantize needing wrap")
    {
        // This test is a bit dangerous as floating errors can lead to sendObjects != recvObjects but the difference is minuscule so we don't care abut such cases
        Rotator2_NetQuantize sendObjects, recvObjects;
        sendObjects.X = -1.87f;
        sendObjects.Y = TiltedPhoques::Pi * 18.0 + 3.6f;

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendObjects.Serialize(writer);

            Buffer::Reader reader(&buff);
            recvObjects.Deserialize(reader);

            REQUIRE(sendObjects == recvObjects);
        }
    }
}

TEST_CASE("Differential structures", "[encoding.differential]")
{
    GIVEN("Full ActionEvent")
    {
        ActionEvent sendAction, recvAction;

        sendAction.ActionId = 42;
        sendAction.State1 = 6547;
        sendAction.Tick = 48;
        sendAction.ActorId = 12345678;
        sendAction.EventName = "test";
        sendAction.IdleId = 87964;
        sendAction.State2 = 8963;
        sendAction.TargetEventName = "toast";
        sendAction.TargetId = 963741;
        sendAction.Type = 4;

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendAction.GenerateDifferential(recvAction, writer);

            Buffer::Reader reader(&buff);
            recvAction.ApplyDifferential(reader);

            REQUIRE(sendAction == recvAction);
        }

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendAction.EventName = "Plot twist !";

            sendAction.GenerateDifferential(recvAction, writer);

            Buffer::Reader reader(&buff);
            recvAction.ApplyDifferential(reader);

            REQUIRE(sendAction == recvAction);
        }
    }

    GIVEN("Full Mods")
    {
        Mods sendMods, recvMods;

        Buffer buff(1000);
        Buffer::Writer writer(&buff);

        sendMods.StandardMods.push_back({ "Hello", 42 });
        sendMods.StandardMods.push_back({ "Hi", 14 });
        sendMods.LiteMods.push_back({ "Test", 8 });
        sendMods.LiteMods.push_back({ "Toast", 49 });

        sendMods.Serialize(writer);

        Buffer::Reader reader(&buff);
        recvMods.Deserialize(reader);

        REQUIRE(sendMods == recvMods);
    }
}

TEST_CASE("Packets", "[encoding.packets]")
{
    SECTION("AuthenticationRequest")
    {
        Buffer buff(1000);

        AuthenticationRequest sendMessage, recvMessage;
        sendMessage.Token = "TesSt";
        sendMessage.Mods.StandardMods.push_back({ "Hello", 42 });
        sendMessage.Mods.StandardMods.push_back({ "Hi", 14 });
        sendMessage.Mods.LiteMods.push_back({ "Test", 8 });
        sendMessage.Mods.LiteMods.push_back({ "Toast", 49 });

        Buffer::Writer writer(&buff);
        sendMessage.Serialize(writer);

        Buffer::Reader reader(&buff);

        uint64_t trash;
        reader.ReadBits(trash, 8); // pop opcode

        recvMessage.DeserializeRaw(reader);

        REQUIRE(sendMessage == recvMessage);
    }

    SECTION("AuthenticationResponse")
    {
        Buffer buff(1000);

        AuthenticationResponse sendMessage, recvMessage;
        sendMessage.Accepted = true;
        sendMessage.Mods.StandardMods.push_back({ "Hello", 42 });
        sendMessage.Mods.StandardMods.push_back({ "Hi", 14 });
        sendMessage.Mods.LiteMods.push_back({ "Test", 8 });
        sendMessage.Mods.LiteMods.push_back({ "Toast", 49 });
        sendMessage.Scripts.Data.push_back(1);
        sendMessage.Scripts.Data.push_back(2);
        sendMessage.ReplicatedObjects.Data.push_back(3);
        sendMessage.ReplicatedObjects.Data.push_back(4);

        Buffer::Writer writer(&buff);
        sendMessage.Serialize(writer);

        Buffer::Reader reader(&buff);

        uint64_t trash;
        reader.ReadBits(trash, 8); // pop opcode

        recvMessage.DeserializeRaw(reader);

        REQUIRE(sendMessage == recvMessage);
    }

    SECTION("CancelAssignmentRequest")
    {
        Buffer buff(1000);

        CancelAssignmentRequest sendMessage, recvMessage;
        sendMessage.Cookie = 14523698;
        Buffer::Writer writer(&buff);
        sendMessage.Serialize(writer);

        Buffer::Reader reader(&buff);

        uint64_t trash;
        reader.ReadBits(trash, 8); // pop opcode

        recvMessage.DeserializeRaw(reader);

        REQUIRE(sendMessage == recvMessage);
    }

    SECTION("RemoveCharacterRequest")
    {
        Buffer buff(1000);

        RemoveCharacterRequest sendMessage, recvMessage;
        sendMessage.ServerId = 14523698;
        Buffer::Writer writer(&buff);
        sendMessage.Serialize(writer);

        Buffer::Reader reader(&buff);

        uint64_t trash;
        reader.ReadBits(trash, 8); // pop opcode

        recvMessage.DeserializeRaw(reader);

        REQUIRE(sendMessage == recvMessage);
    }

    SECTION("AssignCharacterRequest")
    {
        Buffer buff(1000);

        ActionEvent sendAction;
        sendAction.ActionId = 42;
        sendAction.State1 = 6547;
        sendAction.Tick = 48;
        sendAction.ActorId = 12345678;
        sendAction.EventName = "test";
        sendAction.IdleId = 87964;
        sendAction.State2 = 8963;
        sendAction.TargetEventName = "toast";
        sendAction.TargetId = 963741;
        sendAction.Type = 4;

        AssignCharacterRequest sendMessage, recvMessage;
        sendMessage.Cookie = 14523698;
        sendMessage.AppearanceBuffer = "toto";
        sendMessage.CellId.BaseId = 45;
        sendMessage.FormId.ModId = 48;
        sendMessage.ReferenceId.BaseId = 456799;
        sendMessage.ReferenceId.ModId = 4079;
        sendMessage.LatestAction = sendAction;
        sendMessage.Position.m_x = -452.4f;
        sendMessage.Position.m_y = 452.4f;
        sendMessage.Position.m_z = 125452.4f;
        sendMessage.Rotation.X = -1.87f;
        sendMessage.Rotation.Y = 45.35f;

        Buffer::Writer writer(&buff);
        sendMessage.Serialize(writer);

        Buffer::Reader reader(&buff);

        uint64_t trash;
        reader.ReadBits(trash, 8); // pop opcode

        recvMessage.DeserializeRaw(reader);

        REQUIRE(sendMessage == recvMessage);
    }
}
