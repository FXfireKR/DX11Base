#include "pch.h"
#include "CDebugOverlay.h"

void CDebugOverlay::Render()
{
#ifdef IMGUI_ACTIVATE
    if (!m_bOpen)
        return;

    const DebugStatsSnapshot& s = dbg.GetSnapshot();
    const DebugHistory& h = dbg.GetHistory();

    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
    ImGui::Begin("Debug Overlay", &m_bOpen, flags);

    if (ImGui::CollapsingHeader("Frame", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("FPS           : %.1f", s.frame.fps);
        ImGui::Text("Frame ms      : %.3f", s.frame.frameMs);
        ImGui::Text("Update ms     : %.3f", s.frame.updateMs);
        ImGui::Text("LateUpdate ms : %.3f", s.frame.lateUpdateMs);
        ImGui::Text("RenderBuild   : %.3f", s.frame.renderBuildMs);
        ImGui::Text("RenderExec    : %.3f", s.frame.renderExecuteMs);
        ImGui::Text("Present ms    : %.3f", s.frame.presentMs);

        ImGui::PlotLines("Frame ms", h.frameMs, DebugHistory::kMaxSamples, h.head, nullptr, 0.f, 40.f, ImVec2(320, 80));
    }

    if (ImGui::CollapsingHeader("World", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Loaded Columns        : %d", s.world.loadedColumnCount);
        ImGui::Text("Loaded Sections       : %d", s.world.loadedSectionCount);
        ImGui::Text("Visible Sections      : %d", s.world.visibleSectionCount);
        ImGui::Text("Hidden Sections       : %d", s.world.hiddenSectionCount);
        ImGui::Text("Culled Sections       : %d", s.world.culledSectionCount);
        ImGui::Text("Dirty Sections        : %d", s.world.dirtySectionCount);
        ImGui::Text("Rebuild Queued        : %d", s.world.rebuildQueuedCount);
        ImGui::Text("Rebuilt This Frame    : %d", s.world.rebuiltThisFrameCount);
        ImGui::Text("Modified Columns      : %d", s.world.modifiedColumnCount);
        ImGui::Text("Modified Cells        : %d", s.world.modifiedCellCount);
        ImGui::Text("Chunk Load This Frame : %d", s.world.chunkLoadCountThisFrame);
        ImGui::Text("Chunk Unload This Frm : %d", s.world.chunkUnloadCountThisFrame);
        ImGui::Text("Block Edit This Frame : %d", s.world.blockEditCountThisFrame);

        ImGui::PlotLines("Visible Sections", h.visibleSections, DebugHistory::kMaxSamples, h.head, nullptr, 0.f, 512.f, ImVec2(320, 80));
        ImGui::PlotLines("Rebuild Queue", h.rebuildQueue, DebugHistory::kMaxSamples, h.head, nullptr, 0.f, 128.f, ImVec2(320, 80));
    }

    if (ImGui::CollapsingHeader("Render", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Submitted RenderItems : %d", s.render.submittedRenderItemCount);
        ImGui::Text("Draw Calls Total      : %d", s.render.drawCallCount);
        ImGui::Text("Draw Calls Shadow     : %d", s.render.drawCallCountShadow);
        ImGui::Text("Draw Calls Sky        : %d", s.render.drawCallCountSky);
        ImGui::Text("Draw Calls Opaque     : %d", s.render.drawCallCountOpaque);
        ImGui::Text("Draw Calls Cutout     : %d", s.render.drawCallCountCutout);
        ImGui::Text("Draw Calls Translucent: %d", s.render.drawCallCountTranslucent);
        ImGui::Text("Draw Calls Debug      : %d", s.render.drawCallCountDebug);
        ImGui::Text("Draw Calls UI         : %d", s.render.drawCallCountUI);
        ImGui::Text("Pipeline Bind Count   : %d", s.render.pipelineBindCount);
        ImGui::Text("Material Bind Count   : %d", s.render.materialBindCount);
        ImGui::Text("Mesh Bind Count       : %d", s.render.meshBindCount);

        ImGui::PlotLines("Draw Calls", h.drawCalls, DebugHistory::kMaxSamples, h.head, nullptr, 0.f, 2048.f, ImVec2(320, 80));
    }

    if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Player Pos      : %.2f, %.5f, %.2f", s.player.playerPos.x, s.player.playerPos.y, s.player.playerPos.z);
        ImGui::Text("Chunk Coord     : %d, %d, %d", s.player.currentChunkCoord.x, s.player.currentChunkCoord.y, s.player.currentChunkCoord.z);
        ImGui::Text("Block Coord     : %d, %d, %d", s.player.currentBlockCoord.x, s.player.currentBlockCoord.y, s.player.currentBlockCoord.z);
        ImGui::Text("Has Block Hit   : %s", s.player.hasBlockHit ? "true" : "false");

        if (s.player.hasBlockHit)
        {
            ImGui::Text("Hit Block       : %d, %d, %d", s.player.hitBlock.x, s.player.hitBlock.y, s.player.hitBlock.z);
            ImGui::Text("Hit Normal      : %d, %d, %d", s.player.hitNormal.x, s.player.hitNormal.y, s.player.hitNormal.z);
            ImGui::Text("Target Block ID : %u", s.player.targetBlockId);
            ImGui::Text("Target StateIdx : %u", (unsigned)s.player.targetStateIndex);
        }
    }

    ImGui::End();
#endif // IMGUI_ACTIVATE
}