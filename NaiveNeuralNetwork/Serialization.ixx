#include <filesystem>

export module Serialization;

import std.core;

import ActivationFunctions;
import CommandLineInput;
import CostFunctions;
import Hyperparameters;
import Matrix;
import Model;

export namespace Serialization
{
	void WriteModel(std::filesystem::path Path, Model& Parameters)
	{
		std::ofstream OutputModel{ Path, std::ios::binary };
		int LayerCount{ static_cast<int>(Parameters.Hypers.NeuronsPerLayer.size()) };
		OutputModel.write(reinterpret_cast<char*>(&LayerCount), sizeof(LayerCount));

		OutputModel.write(reinterpret_cast<char*>(Parameters.Hypers.NeuronsPerLayer.data()), LayerCount * sizeof(int));

		int ActivationFamilyCount{ static_cast<int>(Parameters.Hypers.ActivationFamilies.size()) };
		OutputModel.write(reinterpret_cast<char*>(&ActivationFamilyCount), sizeof(ActivationFamilyCount));

		for (auto& ActivationFamily : Parameters.Hypers.ActivationFamilies)
		{
			int FamilyNameLength{ static_cast<int>(ActivationFamily.FamilyName.size()) };
			OutputModel.write(reinterpret_cast<char*>(&FamilyNameLength), sizeof(FamilyNameLength));
			OutputModel.write(ActivationFamily.FamilyName.data(), FamilyNameLength);
		}

		int CostFamilyNameLength{ static_cast<int>(Parameters.Hypers.CostFamily.FamilyName.size()) };
		OutputModel.write(reinterpret_cast<char*>(&CostFamilyNameLength), sizeof(CostFamilyNameLength));
		OutputModel.write(Parameters.Hypers.CostFamily.FamilyName.data(), CostFamilyNameLength);

		int MatrixCount{ static_cast<int>(Parameters.Parameters.size()) };
		OutputModel.write(reinterpret_cast<char*>(&MatrixCount), sizeof(MatrixCount));
		for (auto& M : Parameters.Parameters)
		{
			int MatrixWidth{ M.Width };
			OutputModel.write(reinterpret_cast<char*>(&MatrixWidth), sizeof(MatrixWidth));
			int MatrixHeight{ M.Height };
			OutputModel.write(reinterpret_cast<char*>(&MatrixHeight), sizeof(MatrixHeight));
			int MatrixSize{ M.Size() };
			OutputModel.write(reinterpret_cast<char*>(M.Data.data()), MatrixSize * sizeof(float));
		}
	}

	Model ReadModel(std::filesystem::path Path)
	{
		std::ifstream InputModel{ Path, std::ios::binary };
		int LayerCount;
		InputModel.read(reinterpret_cast<char*>(&LayerCount), sizeof(LayerCount));
		std::vector<int> NeuronsPerLayer;
		NeuronsPerLayer.resize(LayerCount);
		InputModel.read(reinterpret_cast<char*>(NeuronsPerLayer.data()), sizeof(int) * LayerCount);

		int ActivationFamilyCount;
		InputModel.read(reinterpret_cast<char*>(&ActivationFamilyCount), sizeof(ActivationFamilyCount));

		std::vector<std::string> ActivationFamilyNames(ActivationFamilyCount);
		for (auto& ActivationFamilyName : ActivationFamilyNames)
		{
			int FamilyNameLength;
			InputModel.read(reinterpret_cast<char*>(&FamilyNameLength), sizeof(FamilyNameLength));
			ActivationFamilyName.resize(FamilyNameLength);
			InputModel.read(ActivationFamilyName.data(), FamilyNameLength);
		}

		int CostFamilyNameLength;
		InputModel.read(reinterpret_cast<char*>(&CostFamilyNameLength), sizeof(CostFamilyNameLength));
		std::string CostFamilyName;
		CostFamilyName.resize(CostFamilyNameLength);
		InputModel.read(CostFamilyName.data(), CostFamilyNameLength);

		std::vector<ActivationFunctions::ActivationFamily> ActivationFamilies;
		std::ranges::transform(ActivationFamilyNames.begin(), ActivationFamilyNames.end(), std::back_inserter(ActivationFamilies), [](const std::string_view FamilyName)
		{
			return *ActivationFunctions::FamiliesByName.find(FamilyName)->second;
		});

		Hyperparameters Hypers
		{
			std::move(NeuronsPerLayer),
			std::move(ActivationFamilies),
			*(CostFunctions::FamiliesByName.find(CostFamilyName)->second)
		};

		int MatrixCount;
		InputModel.read(reinterpret_cast<char*>(&MatrixCount), sizeof(MatrixCount));
		std::vector<Matrix> Matrices(MatrixCount);
		for (auto& M : Matrices)
		{
			InputModel.read(reinterpret_cast<char*>(&M.Width), sizeof(M.Width));
			InputModel.read(reinterpret_cast<char*>(&M.Height), sizeof(M.Height));
			int MatrixSize{ M.Size() };
			M.Data.resize(MatrixSize);
			InputModel.read(reinterpret_cast<char*>(M.Data.data()), MatrixSize * sizeof(float));
		}

		return { std::move(Hypers), std::move(Matrices) };
	}

	Model InitializeModel(const CommandLineInput::CommandLineParams& CommandLine)
	{
		if (CommandLine.InputModelPath.has_value())
		{
			return ReadModel(*CommandLine.InputModelPath);
		}
		else
		{
			std::vector<ActivationFunctions::ActivationFamily> ActivationFamilies;
			std::ranges::transform(CommandLine.ActivationFamilies.begin(), CommandLine.ActivationFamilies.end(), std::back_inserter(ActivationFamilies), [](const std::string_view FamilyName)
			{
				return *ActivationFunctions::FamiliesByName.find(FamilyName)->second;
			});

			Hyperparameters Hypers
			{
				CommandLine.NeuronsPerLayer,
				std::move(ActivationFamilies),
				*(CostFunctions::FamiliesByName.find(*CommandLine.CostFamily)->second)
			};

			//TODO: validate hyperparameters

			return { std::move(Hypers) };
		}
	}
}
